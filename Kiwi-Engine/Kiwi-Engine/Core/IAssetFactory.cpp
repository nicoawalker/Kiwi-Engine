#include "IAssetFactory.h"

#include "EngineRoot.h"

namespace Kiwi
{

	/*creates a new instance of an asset*/
	std::unique_ptr<Kiwi::IAsset> IAssetFactory::CreateAsset( std::wstring assetName )
	{
		std::lock_guard<std::recursive_mutex> lockGuard( m_factoryMutex );

		std::vector<std::unique_ptr<Kiwi::IAsset>> createdAssets;

		this->_CreateAsset( createdAssets, assetName, Kiwi::StringPair {} );
		if( createdAssets.size() == 1 )
		{
			return std::move( createdAssets[0] );

		} else if( createdAssets.size() > 1 )
		{
			for( unsigned int i = 1; i < createdAssets.size(); i++ )
			{
				_kAssetManager._InsertAsset( std::move( createdAssets[i] ) );
			}

			return std::move( createdAssets[0] );
		}

		return nullptr;
	}

	/*creates a new instance of an asset*/
	std::unique_ptr<Kiwi::IAsset> IAssetFactory::CreateAsset( std::wstring assetName, const Kiwi::StringPair& parameters )
	{
		std::lock_guard<std::recursive_mutex> lockGuard( m_factoryMutex );

		std::vector<std::unique_ptr<Kiwi::IAsset>> createdAssets;

		this->_CreateAsset( createdAssets, assetName, parameters ); 
		if( createdAssets.size() == 1 )
		{
			return std::move( createdAssets[0] );

		} else if(createdAssets.size() > 1)
		{
			for( unsigned int i = 1; i < createdAssets.size(); i++ )
			{
				_kAssetManager._InsertAsset( std::move( createdAssets[i] ) );
			}

			return std::move( createdAssets[0] );
		}

		return nullptr;
	}

}