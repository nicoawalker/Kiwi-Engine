#include "IUIButtonEventEmitter.h"

namespace Kiwi
{

	void IUIButtonEventEmitter::_EmitUIButtonEventQueue()
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_EmitEventQueue();
	}

	void IUIButtonEventEmitter::DisconnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void IUIButtonEventEmitter::DisconnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::wstring& eventType )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_Disconnect( receiver, eventType );
	}

	void IUIButtonEventEmitter::DisconnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void IUIButtonEventEmitter::EnqueueUIButtonEvent( std::shared_ptr<UIButtonEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_EnqueueEvent( evt->eventCategory, evt );
	}

	void IUIButtonEventEmitter::EmitUIButtonEvent( std::shared_ptr<UIButtonEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void IUIButtonEventEmitter::ConnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<UIButtonEvent> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_Connect( receiver, eventType, func );
	}

	void IUIButtonEventEmitter::ConnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<UIButtonEvent> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );
	}

}