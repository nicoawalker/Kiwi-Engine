#include "IRenderEventEmitter.h"

namespace Kiwi
{

	void IRenderEventEmitter::_EmitRenderEventQueue()
	{
		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_EmitEventQueue();
	}

	void IRenderEventEmitter::DisconnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void IRenderEventEmitter::DisconnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::wstring& eventType )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_Disconnect( receiver, eventType );
	}

	void IRenderEventEmitter::DisconnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void IRenderEventEmitter::EnqueueRenderEvent( std::shared_ptr<RenderEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_EnqueueEvent( evt->eventCategory, evt );
	}

	void IRenderEventEmitter::EmitRenderEvent( std::shared_ptr<RenderEvent> evt )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void IRenderEventEmitter::ConnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<RenderEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_Connect( receiver, eventType, func );

	}

	void IRenderEventEmitter::ConnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<RenderEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_renderEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );

	}

}