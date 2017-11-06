#ifndef _KIWI_SHADERFACTORY_H_
#define _KIWI_SHADERFACTORY_H_

#include "..\Core\IAssetFactory.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class ShaderFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		ShaderFactory() {}
		virtual ~ShaderFactory() {}

		Kiwi::ShaderFactory* Clone()const { return new Kiwi::ShaderFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif
