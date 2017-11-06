#ifndef _KIWI_COLLIDER_H_
#define _KIWI_COLLIDER_H_

#include "../Core/Component.h"

#include <unordered_set>

namespace Kiwi
{

	class Rigidbody;
	class Scene;

	class Collider:
		public Kiwi::Component
	{
	public:

		enum COLLIDER_TYPE { COLLIDER_SPHERE };

	protected:

		COLLIDER_TYPE m_colliderType;

		std::unordered_set<Kiwi::Collider*> m_activeCollisions;

		bool m_isTrigger;

	public:

		Collider( std::wstring identifier, Kiwi::Scene& scene );
		virtual ~Collider() = 0;

		/*adds the collider to the active collisions list and triggers a collision event
		but does not check the collision*/
		virtual void AddCollision( Kiwi::Collider* collider ) {}

		virtual bool CheckCollision( Kiwi::Collider& collider ) = 0;

		void SetTrigger( bool isTrigger ) { m_isTrigger = isTrigger; }

		COLLIDER_TYPE GetType()const { return m_colliderType; }
		bool IsTrigger()const { return m_isTrigger; }

	};
}

#endif