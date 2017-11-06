#pragma once
#ifndef _KIWI_MATERIALFACTORY_H_
#define _KIWI_MATERIALFACTORY_H_

#include "IAssetFactory.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class MaterialFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		MaterialFactory() {}
		virtual ~MaterialFactory() {}

		Kiwi::MaterialFactory* Clone()const { return new Kiwi::MaterialFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif