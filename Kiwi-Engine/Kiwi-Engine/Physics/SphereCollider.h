#ifndef _KIWI_SPHERECOLLIDER_H_
#define _KIWI_SPHERECOLLIDER_H_

#include "Collider.h"

namespace Kiwi
{

	class Scene;

	class SphereCollider :
		public Kiwi::Collider
	{
	protected:

		double m_radius;

	public:

		SphereCollider( Kiwi::Scene& scene, double radius );
		~SphereCollider() {}

		bool CheckCollision( Kiwi::Collider& collider );

		void SetRadius( double radius ) { m_radius = radius; }

		double GetRadius()const { return m_radius; }

	};
}

#endif