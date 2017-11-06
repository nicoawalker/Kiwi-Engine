#ifndef _KIWI_STATICMODELFACTORY_H_
#define _KIWI_STATICMODELFACTORY_H_

#include "IAssetFactory.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class StaticModelFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

		void _LoadModel( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const std::wstring& modelFile, bool isShared );

	public:

		StaticModelFactory() {}
		virtual ~StaticModelFactory() {}

		Kiwi::StaticModelFactory* Clone()const { return new Kiwi::StaticModelFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif