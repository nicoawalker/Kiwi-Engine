#include "EngineRoot.h"

#include <Windows.h>

namespace Kiwi
{

	EngineRoot::EngineRoot() :
		m_sceneManager( this )
	{

	}

	EngineRoot::~EngineRoot()
	{

	}

	void EngineRoot::_MainLoop()
	{

		m_engineRunning = true;
		while( m_engineRunning )
		{
			this->_PumpMessages();
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