#ifndef _KIWI_BASIC2DSHADERFACTORY_H_
#define _KIWI_BASIC2DSHADERFACTORY_H_

#include "..\Core\IAssetFactory.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class Basic2DShaderFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		Basic2DShaderFactory() {}
		virtual ~Basic2DShaderFactory() {}

		Kiwi::Basic2DShaderFactory* Clone()const { return new Kiwi::Basic2DShaderFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif
