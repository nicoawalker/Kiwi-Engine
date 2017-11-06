#include "CollisionEvent.h"

namespace Kiwi
{

	CollisionEvent::CollisionEvent( Kiwi::Collider& source, Kiwi::Collider& target, CollisionType type ) :
		Kiwi::IEvent<Collider>( &source, L"CollisionEvent" )
	{

		targetCollider = &target;
		collisionType = type;
		isTrigger = (collisionType == TRIGGER_ENTER || collisionType == TRIGGER_EXIT || collisionType == TRIGGER_SUSTAINED) ? true : false;

	}

}