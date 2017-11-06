#ifndef _KIWI_COLLISIONEVENT_H_
#define _KIWI_COLLISIONEVENT_H_

#include "Event.h"

#include <Windows.h>

namespace Kiwi
{

	class Collider;

	class CollisionEvent: 
		public Kiwi::Event<Kiwi::Collider>
	{
	public:

		enum COLLISION_STATE { COLLISION_ENTER, COLLISION_EXIT, COLLISION_SUSTAINED, TRIGGER_ENTER, TRIGGER_EXIT, TRIGGER_SUSTAINED };

	protected:

		Kiwi::Collider* m_target;

		COLLISION_STATE m_collisionState;

	public:

		/*A collision event between 'source' and 'target', broadcast by 'source'*/
		CollisionEvent(Kiwi::Collider& source, Kiwi::Collider& target, COLLISION_STATE collisionState ) :
			Event(&source),
			m_target(&target), m_collisionState(collisionState){}

		virtual ~CollisionEvent() {}

		Kiwi::Collider* GetTarget()const { return m_target; }
		COLLISION_STATE GetState()const { return m_collisionState; }

	};
};

#endif