#include "Rigidbody.h"

#include "../Core/Entity.h"
#include "../Core/Transform.h"
#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Scene.h"
#include "../Core/EngineRoot.h"
#include "../Core/Console.h"
#include "../Core/Math.h"

#include "PhysicsSystem.h"

namespace Kiwi
{

	Rigidbody::Rigidbody( Kiwi::Scene& scene ):
		Kiwi::Component( L"Rigidbody", Kiwi::ComponentType::Rigidbody, scene )
	{

		m_collider = 0;
		m_mass = 1.0;
		m_isActive = true;
		m_frictionStatic = 1.0;
		m_isKinematic = true;

	}

	Rigidbody::~Rigidbody()
	{
		
	}

	void Rigidbody::_OnFixedUpdate()
	{

		if( !m_isActive || !m_entity )
		{
			return;
		}

		if( m_mass == 0.0 )
		{
			return;
		}

		Kiwi::PhysicsSystem* physics = m_entity->GetScene()->GetPhysicsSystem();
		assert( physics != 0 );
		Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );

		if( transform )
		{
			if( m_isKinematic )
			{
				double fixedDeltaTime = _kEngine.GetGameTimer().GetFixedDeltaTime();
				Kiwi::Vector3d gravity = physics->GetGravity();

				m_appliedForce += gravity * m_mass * fixedDeltaTime;

				//Kiwi::Vector3d Ff;
				//if( m_velocity.x > 0.0 || m_velocity.z > 0.0 )
				//{
				//	double Fn = std::abs( gravity.y * m_mass ); //normal force
				//	Kiwi::Vector3d temp = m_velocity;
				//	temp.y = 0.0;
				//	Kiwi::Vector3d Ff = (temp.Normalized() * -1.0) * (Fn * m_frictionStatic); //frictional force
				//	Ff.y = 0.0;
				//	//temp = m_appliedForce;
				//	//temp.y = 0.0;
				//	//Ff.ClampMagnitude( m_velocity.Magnitude() );
				//	m_appliedForce += Ff;
				//}

				//Velocity verlet integration
				Kiwi::Vector3d lastAccel = m_acceleration;
				transform->Translate( m_velocity * fixedDeltaTime );
				m_acceleration = (m_appliedForce / m_mass);
				Kiwi::Vector3d avgAccel = (lastAccel + m_acceleration) / 2.0;
				m_velocity += avgAccel;

				m_appliedForce.Set( 0.0, 0.0, 0.0 );
				m_exertedForce = m_acceleration * m_mass;
			}
		}

	}

	void Rigidbody::_OnShutdown()
	{

	}

	void Rigidbody::_OnAttached()
	{
	}

	Kiwi::Component* Rigidbody::_AttachComponent( std::unique_ptr<Kiwi::Component> component )
	{
		Kiwi::Component* existingComponent = this->_FindComponent( component->GetName() );
		if( existingComponent != 0 )
		{//component already exists
			/*check if the new component is a collider to replace the current one, otherwise let the default _AttachComponent() function handle this component*/
			Kiwi::Collider* collider = dynamic_cast<Kiwi::Collider*>(component.get());
			if( collider != 0 )
			{
				if( m_collider )
				{
					if( m_collider == collider )
					{
						return m_collider;
					}

					this->_DetachComponent( m_collider );
					m_collider = 0;
				}

				m_collider = collider;
			} 
		}

		return Component::_AttachComponent( std::move( component ) );

	}

	/*Kiwi::Collider* Rigidbody::AttachComponent( Kiwi::Collider* collider )
	{

		if( collider )
		{
			if( m_collider )
			{
				if( m_collider == collider )
				{
					return m_collider;
				}

				this->DetachComponent( m_collider->GetID() );
			}

			this->_AttachComponent( collider );

			m_collider = collider;
			m_collider->AddCollisionEventListener( this );
		}

		return collider;

	}*/

}