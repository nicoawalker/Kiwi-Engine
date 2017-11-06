#ifndef _KIWI_BASICINSTANCESHADERFACTORY_H_
#define _KIWI_BASICINSTANCESHADERFACTORY_H_

#include "..\Core\IAssetFactory.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class BasicInstanceShaderFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		BasicInstanceShaderFactory() {}
		virtual ~BasicInstanceShaderFactory() {}

		Kiwi::BasicInstanceShaderFactory* Clone()const { return new Kiwi::BasicInstanceShaderFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif