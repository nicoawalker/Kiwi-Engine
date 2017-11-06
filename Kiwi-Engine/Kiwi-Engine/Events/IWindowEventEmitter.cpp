#include "IWindowEventEmitter.h"

namespace Kiwi
{

	void IWindowEventEmitter::DisconnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void IWindowEventEmitter::DisconnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::wstring& eventType )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

		this->_Disconnect( receiver, eventType );
	}

	void IWindowEventEmitter::DisconnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void IWindowEventEmitter::EmitWindowEvent( std::shared_ptr<WindowEvent> evt )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void IWindowEventEmitter::ConnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<WindowEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

		this->_Connect( receiver, eventType, func );

	}

	void IWindowEventEmitter::ConnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<WindowEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );

	}

}