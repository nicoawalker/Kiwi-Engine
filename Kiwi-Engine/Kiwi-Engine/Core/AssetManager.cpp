#include "AssetManager.h"

#include "IAsset.h"

#include "Utilities.h"

namespace Kiwi
{

	void AssetManager::AddAsset( Kiwi::IAsset* asset )
	{

		if( asset )
		{
			m_assets.Add( asset->GetAssetName(), asset );
		}

	}

}