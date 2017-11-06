#ifndef _KIWI_BASICSHADERFACTORY_H_
#define _KIWI_BASICSHADERFACTORY_H_

#include "..\Core\IAssetFactory.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class BasicShaderFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		BasicShaderFactory() {}
		virtual ~BasicShaderFactory() {}

		Kiwi::BasicShaderFactory* Clone()const { return new Kiwi::BasicShaderFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif
