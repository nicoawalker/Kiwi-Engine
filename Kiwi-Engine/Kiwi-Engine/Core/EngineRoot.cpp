#include "EngineRoot.h"
#include "Console.h"
#include "Exception.h"
#include "IEngineApp.h"

#include "TextureFactory.h"
#include "FontFactory.h"
#include "RigidbodyComponentFactory.h"
#include "StaticModelFactory.h"
#include "MD5ModelFactory.h"
#include "MaterialFactory.h"

#include "Threading\LoggingWorker.h"

#include "..\Types.h"

#include "../Factories/BasicShaderFactory.h"
#include "..\Factories\Basic2DShaderFactory.h"
#include "../Factories/ShaderFactory.h"
#include "..\Factories\BasicInstanceShaderFactory.h"

#include "../Graphics/UI/WidgetFactory.h"

#include <fstream>
#include <ctime>

#include <Windows.h>

namespace Kiwi
{

	EngineRoot::EngineRoot()
	{
		m_gameTimer.SetTargetUpdatesPerSecond( 60 );

		m_globalUIScale = 1.0;
		m_initialized = false;
		m_shutdown = false;
		m_engineRunning = false;
	}

	EngineRoot::~EngineRoot()
	{
		if( m_shutdown == false )
		{
			MessageBox( NULL, L"Engine not shutdown properly", L"Engine not shutdown properly", MB_OK );
		}
	}

	void EngineRoot::_MainLoop()
	{
		m_gameTimer.StartTimer();
		m_gameTimer.Update();

		m_engineRunning = true;
		while( m_engineRunning )
		{
			m_gameTimer.Update();

			m_threadPool.Update();

			m_inputTranslator.Update();

			m_assetManager.Update();

			/*update render windows, which will update keyboard and mouse input*/
			m_graphicsManager.Update();

			if( this->_PumpMessages() == false ) break;

			//if enough time has passed, send a fixed update
			if( m_gameTimer.QueryFixedUpdate() )
			{
				this->EmitFrameEvent( std::make_shared<Kiwi::FrameEvent>( *this, FrameEventType::FIXED_UPDATE ) );

				m_physicsSystem.FixedUpdate();
			}

			this->EmitFrameEvent( std::make_shared<Kiwi::FrameEvent>( *this, FrameEventType::UPDATE ) );

			m_sceneManager.Render();
		}
		
		this->Shutdown();
	}

	bool EngineRoot::_PumpMessages()
	{
		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( msg.message == WM_QUIT  )
			{
				return false;
			}

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		return true;
	}

	void EngineRoot::Initialize( std::wstring logFile )
	{
		std::lock_guard<std::recursive_mutex> lockGuard( m_engineMutex );

		/*start the output logging thread*/
		m_logFile = logFile;
		m_logWorker = std::make_shared<LoggingWorker>( logFile );

		m_threadPool.RunAsync( m_logWorker );

		/*create and add the default factories for creating game objects*/
		this->RegisterNewFactory( std::unique_ptr<Kiwi::RigidbodyComponentFactory>( new Kiwi::RigidbodyComponentFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::WidgetFactory>( new Kiwi::WidgetFactory() ) );

		this->RegisterNewFactory( std::unique_ptr<Kiwi::TextureFactory>( new Kiwi::TextureFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::FontFactory>( new Kiwi::FontFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::BasicShaderFactory>( new Kiwi::BasicShaderFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::BasicInstanceShaderFactory>( new Kiwi::BasicInstanceShaderFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::Basic2DShaderFactory>( new Kiwi::Basic2DShaderFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::ShaderFactory>( new Kiwi::ShaderFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::StaticModelFactory>( new Kiwi::StaticModelFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::MD5ModelFactory>( new Kiwi::MD5ModelFactory() ) );
		this->RegisterNewFactory( std::unique_ptr<Kiwi::MaterialFactory>( new Kiwi::MaterialFactory() ) );

		m_assetManager.Initialize();

		m_initialized = true;
	}

	void EngineRoot::Shutdown()
	{
		std::lock_guard<std::recursive_mutex> lockGuard( m_engineMutex );

		m_threadPool.ShutdownAll();

		m_sceneManager.Shutdown();
		m_physicsSystem.Shutdown();

		m_assetManager.Shutdown();
		m_graphicsManager.Shutdown();

		m_engineRunning = false;
		m_shutdown = true;
	}

	void EngineRoot::Start()
	{
		if( m_engineRunning ) return;

		if( m_initialized == false )
		{
			throw Kiwi::Exception( L"EngineRoot::Start", L"Engine started before initialization. Call EngineRoot::Initialize first", KIWI_EXCEPTION::NOTINITIALIZED );
		}

		this->_MainLoop();
	}

	void EngineRoot::Stop()
	{
		m_engineRunning = false;
	}

	void EngineRoot::Log( std::wstring log )
	{
		std::lock_guard<std::recursive_mutex> lockGuard( m_engineMutex );

		m_logWorker->Log( log );
	}

	void EngineRoot::RegisterNewFactory( std::unique_ptr<Kiwi::IFactory> factory )
	{
		std::lock_guard<std::recursive_mutex> lockGuard( m_engineMutex );

		m_factories.insert( std::make_pair( factory->GetObjectType(), std::move( factory ) ) );
	}

}