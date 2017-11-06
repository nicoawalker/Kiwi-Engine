#include "IEngineApp.h"
#include "Utilities.h"

namespace Kiwi
{

	IEngineApp::IEngineApp()
	{
		m_engine = std::make_unique<Kiwi::EngineRoot>();

		_kEngine.ConnectFrameEventReceiver( *this, L"", &IEngineApp::_OnFrameEvent );
	}

	IEngineApp::~IEngineApp()
	{
	}

	Kiwi::EventResponse IEngineApp::_OnFrameEvent( std::shared_ptr<Kiwi::FrameEvent> evt )
	{
		switch( evt->eventType )
		{
			case FrameEventType::FIXED_UPDATE:
				{
					this->_OnFixedUpdate();
					break;
				}
			case FrameEventType::UPDATE:
				{
					this->_OnUpdate();
					break;
				}
			default: break;
		}

		return Kiwi::EventResponse::NONE;
	}

	void IEngineApp::Shutdown()
	{
		this->_OnShutdown();

		if( Kiwi::EngineRoot::GetSingletonPtr() )
		{
			_kEngine.Stop();
		}
	}

}