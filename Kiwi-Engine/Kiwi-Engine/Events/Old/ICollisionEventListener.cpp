#include "ICollisionEventListener.h"

namespace Kiwi
{

	void ICollisionEventListener::OnCollisionEvent( const Kiwi::CollisionEvent& evt )
	{

		switch( evt.GetState() )
		{
			case Kiwi::CollisionEvent::COLLISION_ENTER:
				{
					this->OnCollisionEnter( evt );
					break;
				}
			case Kiwi::CollisionEvent::COLLISION_EXIT:
				{
					this->OnCollisionExit( evt );
					break;
				}
			case Kiwi::CollisionEvent::COLLISION_SUSTAINED:
				{
					this->OnCollision( evt );
					break;
				}
			case Kiwi::CollisionEvent::TRIGGER_ENTER:
				{
					this->_OnTriggerEnter( evt );
					break;
				}
			case Kiwi::CollisionEvent::TRIGGER_EXIT:
				{
					this->_OnTriggerExit( evt );
					break;
				}
			case Kiwi::CollisionEvent::TRIGGER_SUSTAINED:
				{
					this->_OnTriggerSustained( evt );
					break;
				}
			default: return;
		}

	}

}