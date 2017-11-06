#include "IGameObjectFactory.h"

#include "Scene.h"

#include <memory>

namespace Kiwi
{

	std::unique_ptr<Kiwi::GameObject> IGameObjectFactory::CreateObject( std::wstring name, Kiwi::Scene& scene )
	{

		std::lock_guard<std::recursive_mutex> lockGuard( m_factoryMutex );

		return this->_CreateObject( name, scene, Kiwi::StringPair {} );

	}

	/*creates a new instance of an entity with the given name and initialization parameters, and inserts the entity into the given scene*/
	std::unique_ptr<Kiwi::GameObject> IGameObjectFactory::CreateObject( std::wstring name, Kiwi::Scene& scene, Kiwi::StringPair& parameters )
	{

		std::lock_guard<std::recursive_mutex> lockGuard( m_factoryMutex );

		return this->_CreateObject( name, scene, parameters );

	}

}