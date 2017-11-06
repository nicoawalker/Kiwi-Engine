#include "IInputEventEmitter.h"

namespace Kiwi
{

	void IInputEventEmitter::_EmitInputEventQueue()
	{
		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_EmitEventQueue();
	}

	void IInputEventEmitter::DisconnectInputEventReceiver( IEventReceiver<InputEvent>& receiver )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void IInputEventEmitter::DisconnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::wstring& eventType )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_Disconnect( receiver, eventType );
	}

	void IInputEventEmitter::DisconnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void IInputEventEmitter::EnqueueInputEvent( std::shared_ptr<InputEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_EnqueueEvent( evt->eventCategory, evt );
	}

	void IInputEventEmitter::EmitInputEvent( std::shared_ptr<InputEvent> evt )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void IInputEventEmitter::ConnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<InputEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_Connect( receiver, eventType, func );

	}

	void IInputEventEmitter::ConnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<InputEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );

	}

}