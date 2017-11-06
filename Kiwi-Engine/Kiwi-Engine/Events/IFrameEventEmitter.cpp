#include "IFrameEventEmitter.h"

namespace Kiwi
{

	void IFrameEventEmitter::DisconnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void IFrameEventEmitter::DisconnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::wstring& eventType )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

		this->_Disconnect( receiver, eventType );
	}

	void IFrameEventEmitter::DisconnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void IFrameEventEmitter::EmitFrameEvent( std::shared_ptr<FrameEvent> evt )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void IFrameEventEmitter::ConnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<FrameEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

		this->_Connect( receiver, eventType, func );

	}

	void IFrameEventEmitter::ConnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<FrameEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );

	}

}