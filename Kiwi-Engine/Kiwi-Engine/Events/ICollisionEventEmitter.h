#ifndef _KIWI_ICOLLISIONEVENTEMITTER_H_
#define _KIWI_ICOLLISIONEVENTEMITTER_H_

#include "EventHandling.h"
#include "CollisionEvent.h"

#include <mutex>

namespace Kiwi
{

	class ICollisionEventEmitter :
		private Kiwi::IEventEmitter<CollisionEvent>
	{
	protected:

		std::recursive_mutex m_collisionEventEmitter_M;

	public:

		ICollisionEventEmitter() :
			Kiwi::IEventEmitter<CollisionEvent>( std::initializer_list<std::wstring>( { L"Collider", L"Trigger" } ) )
		{
		}
		~ICollisionEventEmitter() {}

		void DisconnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver );

		void DisconnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::wstring& eventType );

		void DisconnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EmitCollisionEvent( std::shared_ptr<CollisionEvent> evt );

		void ConnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<CollisionEvent> )> func );

		void ConnectCollisionEventReceiver( IEventReceiver<CollisionEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<CollisionEvent> )> func );

		template<typename CallbackClass>
		void ConnectCollisionEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<CollisionEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectCollisionEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<CollisionEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_collisionEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif