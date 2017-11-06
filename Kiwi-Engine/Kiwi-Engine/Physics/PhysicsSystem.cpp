#include "PhysicsSystem.h"
#include "Collider.h"
#include "SphereCollider.h"

#include "../Core/Utilities.h"
#include "..\Core\Transform.h"
#include "../Core/EngineRoot.h"

#include <vector>

namespace Kiwi
{

	PhysicsSystem::PhysicsSystem()
	{
	}

	PhysicsSystem::~PhysicsSystem()
	{

		auto rigItr = m_rigidbodies.begin();
		for( ; rigItr != m_rigidbodies.end(); )
		{
			//SAFE_DELETE( *rigItr );
			rigItr = m_rigidbodies.erase( rigItr );
			continue;
		}

	}

	void PhysicsSystem::Shutdown()
	{

		auto rigItr = m_rigidbodies.begin();
		for( ; rigItr != m_rigidbodies.end(); rigItr++ )
		{
			if( *rigItr != 0 && !(*rigItr)->IsShutdown() )
			{
				(*rigItr)->Shutdown();
			}
		}

	}

	void PhysicsSystem::FixedUpdate()
	{

		float fixedDeltaTime = _kEngine.GetGameTimer().GetFixedDeltaTime();
		
		std::vector<Kiwi::Collider*> colliders;

		for( auto rigItr = m_rigidbodies.begin(); rigItr != m_rigidbodies.end(); )
		{
			if( *rigItr != 0 )
			{
				if( (*rigItr)->IsActive() && (*rigItr)->IsShutdown() == false )
				{
					(*rigItr)->FixedUpdate();

					if( (*rigItr)->GetCollider() != 0 )
					{
						colliders.push_back( (*rigItr)->GetCollider() );
					}

				} else if( (*rigItr)->IsShutdown() || (*rigItr)->GetEntity() == 0 )
				{
					//SAFE_DELETE( *rigItr );
					rigItr = m_rigidbodies.erase( rigItr );
					continue;
				}

			} else
			{
				rigItr = m_rigidbodies.erase( rigItr );
				continue;
			}

			rigItr++;
		}

		for( unsigned int i = 0; i < colliders.size(); i++ )
		{
			for( unsigned int a = 0; a < colliders.size(); a++ )
			{
				if( a != i )
				{
					colliders[i]->CheckCollision( *colliders[a] );
				}
			}
		}

	}

	void PhysicsSystem::AddRigidbody( Kiwi::Rigidbody* rigidbody )
	{

		if( rigidbody && rigidbody->GetEntity() && m_rigidbodies.find(rigidbody) == m_rigidbodies.end() )
		{
			m_rigidbodies.insert( rigidbody );
		}

	}

	void PhysicsSystem::RemoveRigidbody( Kiwi::Rigidbody* rigidbody )
	{

		m_rigidbodies.erase( rigidbody );

	}

}