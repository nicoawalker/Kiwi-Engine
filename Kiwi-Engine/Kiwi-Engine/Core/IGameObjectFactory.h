#ifndef _KIWI_IGAMEOBJECTFACTORY_H_
#define _KIWI_IGAMEOBJECTFACTORY_H_

/*abstract factory class used to create new instances of GameObject classes*/

#include "GameObject.h"
#include "IFactory.h"

#include "..\Types.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class IGameObjectFactory:
		public Kiwi::IFactory
	{
	protected:

		std::recursive_mutex m_factoryMutex;

	protected:

		virtual std::unique_ptr<Kiwi::GameObject> _CreateObject( const std::wstring& name, Kiwi::Scene& scene, const Kiwi::StringPair& parameters ) = 0;

	public:

		IGameObjectFactory() {}
		virtual ~IGameObjectFactory() {}

		virtual IFactory* Clone()const = 0;

		/*creates a new instance of an entity with the given name*/
		virtual std::unique_ptr<Kiwi::GameObject> CreateObject( std::wstring name, Kiwi::Scene& scene );

		/*creates a new instance of an entity with the given name and initialization parameters*/
		virtual std::unique_ptr<Kiwi::GameObject> CreateObject( std::wstring name, Kiwi::Scene& scene, Kiwi::StringPair& parameters );

		/*returns a string identifying the type of object the factory creates*/
		virtual std::wstring GetObjectType()const = 0;

	};

}

#endif