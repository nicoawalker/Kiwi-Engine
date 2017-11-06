#ifndef _KIWI_STATICMESHFACTORY_H_
#define _KIWI_STATICMESHFACTORY_H_

#include "IAssetFactory.h"

#include "..\Graphics\StaticMeshAsset.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class StaticMeshFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		std::unique_ptr<Kiwi::IAsset> _CreateAsset( std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		StaticMeshFactory() {}
		virtual ~StaticMeshFactory() {}

		Kiwi::StaticMeshFactory* Clone()const { return new Kiwi::StaticMeshFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif
