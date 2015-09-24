#include "EngineRoot.h"
#include "Logger.h"

#include <Windows.h>

namespace Kiwi
{

	Kiwi::Logger _Logger;

	EngineRoot::EngineRoot():
		m_sceneManager( this )
	{
	}

	EngineRoot::~EngineRoot()
	{

		_Logger.Shutdown();

	}

	void EngineRoot::_MainLoop()
	{

		m_engineRunning = true;
		while( m_engineRunning )
		{
			this->_PumpMessages();

			if( m_gameWindow ) m_gameWindow->Update();

			//broadcast a new untimed frame event
			Kiwi::FrameEvent untimedFrameEvent( this, Kiwi::FrameEvent::EventType::UNTIMED_EVENT );
			this->BroadcastEvent( untimedFrameEvent );

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