#include "SphereCollider.h"
#include "Rigidbody.h"

#include "../Core/Entity.h"
#include "../Core/Transform.h"
#include "../Core/Utilities.h"

namespace Kiwi
{

	SphereCollider::SphereCollider( Kiwi::Scene& scene, double radius ):
		Kiwi::Collider( L"SphereCollider", scene )
	{

		m_radius = abs( radius );
		m_colliderType = COLLIDER_SPHERE;

	}

	bool SphereCollider::CheckCollision( Kiwi::Collider& collider )
	{

		Kiwi::Transform* parentTransform = m_entity->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		if( !parentTransform )
		{
			return false;
		}

		switch( collider.GetType() )
		{
			case Kiwi::Collider::COLLIDER_SPHERE:
				{
					Kiwi::SphereCollider* sphCol1 = dynamic_cast<Kiwi::SphereCollider*>(&collider);
					
					if( sphCol1 )
					{
						Kiwi::Transform* sph1Trans = sphCol1->GetEntity()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
						double radius1 = sphCol1->GetRadius();

						if( sph1Trans )
						{
							if( sph1Trans->GetDistance( parentTransform ) <= max(radius1, m_radius) )
							{
								if( m_activeCollisions.find( &collider ) != m_activeCollisions.end() )
								{
									if( collider.IsTrigger() )
									{
										//this->BroadcastCollisionEvent( Kiwi::CollisionEvent( *this, collider, CollisionEvent::TRIGGER_SUSTAINED ) );

									} else
									{
										//this->BroadcastCollisionEvent( Kiwi::CollisionEvent( *this, collider, CollisionEvent::COLLISION_SUSTAINED ) );
									}

								} else
								{
									if( collider.IsTrigger() )
									{
										//this->BroadcastCollisionEvent( Kiwi::CollisionEvent( *this, collider, CollisionEvent::TRIGGER_ENTER ) );

									} else
									{
										//this->BroadcastCollisionEvent( Kiwi::CollisionEvent( *this, collider, CollisionEvent::COLLISION_ENTER ) );
									}

									m_activeCollisions.insert( &collider );

									return true;
								}

							} else if( m_activeCollisions.find( &collider ) != m_activeCollisions.end() )
							{
								m_activeCollisions.erase( &collider );

								if( collider.IsTrigger() )
								{
									//this->BroadcastCollisionEvent( Kiwi::CollisionEvent( *this, collider, CollisionEvent::TRIGGER_EXIT ) );

								} else
								{
									//this->BroadcastCollisionEvent( Kiwi::CollisionEvent( *this, collider, CollisionEvent::COLLISION_EXIT ) );
								}
							}
						}
					}
					break;
				}
		}

		return false;

	}

}