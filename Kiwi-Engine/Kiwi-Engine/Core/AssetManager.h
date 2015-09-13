#ifndef _KIWI_ASSETMANAGER_H_
#define _KIWI_ASSETMANAGER_H_

#include "IAsset.h"
#include "ComponentContainer.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Kiwi
{

	class IAsset;

	class AssetManager
	{
	protected:

		//std::unordered_map<std::wstring, Kiwi::IAsset*> m_assets;
		Kiwi::ComponentContainer<std::wstring, Kiwi::IAsset> m_assets;

	public:

		AssetManager() {}
		~AssetManager() {}

		void AddAsset( Kiwi::IAsset* asset );

		void DestroyAsset( Kiwi::IAsset* asset ) { m_assets.Destroy( asset ); }
		void DestroyAssetWithName( std::wstring assetName ) { m_assets.Destroy( assetName ); }

		Kiwi::IAsset* FindAssetWithName( std::wstring assetName ) { return m_assets.Find( assetName ); }

	};
}

#endif