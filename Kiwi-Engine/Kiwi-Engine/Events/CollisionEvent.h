#ifndef _KIWI_COLLISIONEVENT_H_
#define _KIWI_COLLISIONEVENT_H_

#include "IEvent.h"

#include "../Types.h"

namespace Kiwi
{

	class Collider;

	enum CollisionType { COLLISION_ENTER, COLLISION_EXIT, COLLISION_SUSTAINED, TRIGGER_ENTER, TRIGGER_EXIT, TRIGGER_SUSTAINED };

	class CollisionEvent :
		public Kiwi::IEvent<Collider>
	{
	public:

		Kiwi::Collider* targetCollider;

		CollisionType collisionType;

		bool isTrigger;

	public:

		CollisionEvent( Kiwi::Collider& source, Kiwi::Collider& target, CollisionType type );

	};

}

#endif