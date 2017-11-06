#include "ICollisionEventEmitter.h"

namespace Kiwi
{

	void ICollisionEventEmitter::DisconnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void ICollisionEventEmitter::DisconnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::wstring& eventType )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

		this->_Disconnect( receiver, eventType );
	}

	void ICollisionEventEmitter::DisconnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void ICollisionEventEmitter::EmitCollisionEvent( std::shared_ptr<CollisionEvent> evt )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void ICollisionEventEmitter::ConnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<CollisionEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

		this->_Connect( receiver, eventType, func );

	}

	void ICollisionEventEmitter::ConnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<CollisionEvent> )> func )
	{

		std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );

	}

}