#include "EngineRoot.h"
#include "Logger.h"
#include "IConsole.h"
#include "RawInputWrapper.h"

#include <Windows.h>

namespace Kiwi
{

	Kiwi::Logger _Logger;

	EngineRoot::EngineRoot():
		m_sceneManager( this )
	{

		m_console = 0;
		m_gameTimer.SetTargetUpdatesPerSecond( 60 );

	}

	EngineRoot::~EngineRoot()
	{

		SAFE_DELETE( m_console );

		_Logger.Shutdown();

	}

	void EngineRoot::_MainLoop()
	{

		m_gameTimer.StartTimer();
		m_gameTimer.Update();

		m_engineRunning = true;
		while( m_engineRunning )
		{
			m_gameTimer.Update();

			this->_PumpMessages();

			if( m_gameWindow ) m_gameWindow->Update( m_gameTimer.GetDeltaTime() );

			//if enough time has passed, send a fixed update
			if( m_gameTimer.QueryFixedUpdate() )
			{
				this->BroadcastEvent( Kiwi::FrameEvent( this, Kiwi::FrameEvent::EventType::TIMED_EVENT ) );

				if( m_console ) m_console->OnFixedUpdate();
			}

			//broadcast a new untimed frame event
			this->BroadcastEvent( Kiwi::FrameEvent( this, Kiwi::FrameEvent::EventType::UNTIMED_EVENT ) );

			/*if( !m_graphicsCore->RenderFrame() )
			{
			m_engineRunning = false;
			break;
			}*/

			if( m_console ) m_console->OnUpdate();

		}

	}

	void EngineRoot::_PumpMessages()
	{

		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

	}

	void EngineRoot::Start( std::wstring logFile, Kiwi::RenderWindow* gameWindow )
	{

		assert( gameWindow != 0 );

		m_gameWindow = gameWindow;

		_Logger.Initialize( logFile );

		this->_MainLoop();

	}

	void EngineRoot::Stop()
	{

		m_engineRunning = false;

	}

	void EngineRoot::SetMouseSensitivity( const Kiwi::Vector2& sens )
	{

		auto rwItr = m_graphicsManager.m_renderWindows.Front();
		for( ; rwItr != m_graphicsManager.m_renderWindows.Back(); rwItr++ )
		{
			rwItr->second->GetInput()->SetMouseSpeed( sens );
		}

	}

}