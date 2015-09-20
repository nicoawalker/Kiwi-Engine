#include "EngineRoot.h"
#include "Logger.h"

#include <Windows.h>

namespace Kiwi
{

	Kiwi::Logger _Logger;

	EngineRoot::EngineRoot() :
		m_sceneManager( this )
	{

		_Logger.Initialize( L"H:\\Programming\\Projects\\Kiwi-Engine-Demo\\Kiwi Engine Demo\\Kiwi Engine Demo\\Data\\Logs\\debug.txt" );

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

	void EngineRoot::Start()
	{

		this->_MainLoop();

	}

	void EngineRoot::Stop()
	{

		m_engineRunning = false;

	}

}