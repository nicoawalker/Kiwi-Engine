#ifndef _KIWI_ICOLLISIONEVENTBROADCASTER_H_
#define _KIWI_ICOLLISIONEVENTBROADCASTER_H_

#include "EventBroadcaster.h"
#include "ICollisionEventListener.h"

namespace Kiwi
{

	class CollisionEvent;

	class ICollisionEventBroadcaster :
		private Kiwi::EventBroadcaster<Kiwi::ICollisionEventListener, Kiwi::CollisionEvent>
	{
	public:

		ICollisionEventBroadcaster() :
			EventBroadcaster<Kiwi::ICollisionEventListener, Kiwi::CollisionEvent>(&Kiwi::ICollisionEventListener::OnCollisionEvent) {}

		virtual ~ICollisionEventBroadcaster() {}

		void AddCollisionEventListener( Kiwi::ICollisionEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void RemoveCollisionEventListener( Kiwi::ICollisionEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void RemoveAllCollisionEventListeners()
		{
			this->_RemoveAllListeners();
		}

		void BroadcastCollisionEvent( const Kiwi::CollisionEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

	};
};

#endif