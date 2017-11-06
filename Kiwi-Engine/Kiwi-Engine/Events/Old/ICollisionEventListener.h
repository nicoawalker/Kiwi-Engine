#ifndef _KIWI_ICOLLISIONEVENTLISTENER_H_
#define _KIWI_ICOLLISIONEVENTLISTENER_H_

#include "CollisionEvent.h"

namespace Kiwi
{

	class ICollisionEventBroadcaster;

	class ICollisionEventListener
	{
	friend class ICollisionEventBroadcaster;
	private:

		void OnCollisionEvent( const Kiwi::CollisionEvent& evt );

		virtual void _OnTriggerEnter( const Kiwi::CollisionEvent& evt ) {}
		virtual void _OnTriggerExit( const Kiwi::CollisionEvent& evt ) {}
		virtual void _OnTriggerSustained( const Kiwi::CollisionEvent& evt ) {}

	public:

		ICollisionEventListener(){}
		virtual ~ICollisionEventListener(){}

		/*called on the frame that a collision first occurs*/
		virtual void OnCollisionEnter( const Kiwi::CollisionEvent& evt ) {};

		/*called on the frame that a collision first stops*/
		virtual void OnCollisionExit( const Kiwi::CollisionEvent& evt ) {};

		/*called constantly as long as a collision is taking place*/
		virtual void OnCollision( const Kiwi::CollisionEvent& evt ) {};

	};
};

#endif