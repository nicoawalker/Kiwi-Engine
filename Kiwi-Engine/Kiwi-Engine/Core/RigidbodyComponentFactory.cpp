#include "RigidbodyComponentFactory.h"

#include "Scene.h"

namespace Kiwi
{

	std::wstring RigidbodyComponentFactory::ObjectType = L"Rigidbody";

	std::unique_ptr<Kiwi::GameObject> RigidbodyComponentFactory::_CreateObject( const std::wstring& name, Kiwi::Scene& scene, const Kiwi::StringPair& parameters )
	{

		return std::make_unique<Kiwi::Rigidbody>( scene );

	}

}