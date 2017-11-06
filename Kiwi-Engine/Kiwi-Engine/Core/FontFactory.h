#ifndef _KIWI_FONTFACTORY_H_
#define _KIWI_FONTFACTORY_H_

#include "IAssetFactory.h"

#include "..\Graphics\Font.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class FontFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		FontFactory() {}
		virtual ~FontFactory() {}

		Kiwi::FontFactory* Clone()const { return new Kiwi::FontFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif