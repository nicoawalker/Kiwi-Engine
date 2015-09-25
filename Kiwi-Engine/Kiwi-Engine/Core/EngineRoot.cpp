#include "EngineRoot.h"
#include "Logger.h"

#include <Windows.h>

namespace Kiwi
{

	Kiwi::Logger _Logger;

	EngineRoot::EngineRoot():
		m_sceneManager( this )
	{

		m_gameTimer.SetTargetUpdatesPerSecond( 60 );

	}

	EngineRoot::~EngineRoot()
	{

		_Logger.Shutdown();

	}

	void EngineRoot::_MainLoop()
	{

		m_gameTimer.StartTimer();

		m_engineRunning = true;
		while( m_engineRunning )
		{
			m_gameTimer.Update();

			this->_PumpMessages();

			if( m_gameWindow ) m_gameWindow->Update();

			//if enough time has passed, send a fixed update
			if( m_gameTimer.QueryFixedUpdate() )
			{
				this->BroadcastEvent( Kiwi::FrameEvent( this, Kiwi::FrameEvent::EventType::TIMED_EVENT ) );
			}

			//broadcast a new untimed frame event
			//Kiwi::FrameEvent untimedFrameEvent( this, Kiwi::FrameEvent::EventType::UNTIMED_EVENT );
			//this->BroadcastEvent( untimedFrameEvent );
			this->BroadcastEvent( Kiwi::FrameEvent( this, Kiwi::FrameEvent::EventType::UNTIMED_EVENT ) );

			/*if( !m_graphicsCore->RenderFrame() )
			{
			m_engineRunning = false;
			break;
			}*/

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

}