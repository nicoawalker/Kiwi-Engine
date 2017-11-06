#ifndef _KIWI_PHYSICSSYSTEM_H_
#define _KIWI_PHYSICSSYSTEM_H_

#include "Rigidbody.h"

#include <unordered_set>

namespace Kiwi
{

	class EngineRoot;

	class PhysicsSystem
	{
	protected:

		std::unordered_set<Rigidbody*> m_rigidbodies;

		Kiwi::Vector3d m_gravity;

	public:

		PhysicsSystem();
		~PhysicsSystem();

		void Shutdown();

		void FixedUpdate();

		void AddRigidbody( Kiwi::Rigidbody* rigidbody );
		void RemoveRigidbody( Kiwi::Rigidbody* rigidbody );

		void SetGravity( const Kiwi::Vector3d& gravity ) { m_gravity = gravity; }

		Kiwi::Vector3d GetGravity()const { return m_gravity; }

	};
}

#endif