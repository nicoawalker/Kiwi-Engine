#include "Collider.h"
#include "Rigidbody.h"

namespace Kiwi
{

	Collider::Collider( std::wstring identifier, Kiwi::Scene& scene ):
		Kiwi::Component( identifier, Kiwi::ComponentType::Collider, scene )
	{

		m_isTrigger = false;

	}

	Collider::~Collider()
	{
	}

}