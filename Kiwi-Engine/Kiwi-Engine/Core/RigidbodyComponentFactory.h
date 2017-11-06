#ifndef _KIWI_RIGIDBODYCOMPONENTFACTORY_H_
#define _KIWI_RIGIDBODYCOMPONENTFACTORY_H_

#include "IGameObjectFactory.h"
#include "Component.h"

#include "..\Physics\Rigidbody.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class RigidbodyComponentFactory :
		public Kiwi::IGameObjectFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		std::unique_ptr<Kiwi::GameObject> _CreateObject( const std::wstring& name, Kiwi::Scene& scene, const Kiwi::StringPair& parameters );

	public:

		RigidbodyComponentFactory() {}
		virtual ~RigidbodyComponentFactory() {}

		virtual IFactory* Clone()const { return new Kiwi::RigidbodyComponentFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif