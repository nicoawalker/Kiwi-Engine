#ifndef _KIWI_TEXTUREFACTORY_H_
#define _KIWI_TEXTUREFACTORY_H_

#include "IAssetFactory.h"

#include "..\Graphics\Texture.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class TextureFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		TextureFactory() {}
		virtual ~TextureFactory() {}

		Kiwi::TextureFactory* Clone()const { return new Kiwi::TextureFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif