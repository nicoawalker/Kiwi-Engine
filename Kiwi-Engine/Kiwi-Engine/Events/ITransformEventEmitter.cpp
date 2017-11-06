#include "ITransformEventEmitter.h"

namespace Kiwi
{

	void ITransformEventEmitter::DisconnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void ITransformEventEmitter::DisconnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::wstring& eventType )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

		this->_Disconnect( receiver, eventType );
	}

	void ITransformEventEmitter::DisconnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void ITransformEventEmitter::EmitTransformEvent( std::shared_ptr<TransformEvent> evt )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void ITransformEventEmitter::ConnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<TransformEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

		this->_Connect( receiver, eventType, func );

	}

	void ITransformEventEmitter::ConnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<TransformEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );

	}

}