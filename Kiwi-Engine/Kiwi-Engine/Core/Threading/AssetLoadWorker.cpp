#include "AssetLoadWorker.h"

#include "../AssetManager.h"
#include "..\EngineRoot.h"
#include "../Exception.h"
#include "..\IAssetFactory.h"
#include "../AssetLoadQueue.h"

#include "..\..\Types.h"

namespace Kiwi
{

	AssetLoadWorker::AssetLoadWorker( Kiwi::AssetManager& assetManager, QueuedAssetData& assetData )
	{
		m_assetManager = &assetManager;
		m_assetData = std::make_unique<QueuedAssetData>( assetData );
		m_assetID = 0;
	}

	void AssetLoadWorker::Run()
	{	
		m_ready = true;

		if( m_assetData == nullptr ) return;

		/*an asset definition was returned, proceed to load it*/
		std::wstring assetType = m_assetData->assetCategory;
		std::wstring assetName = m_assetData->assetName;
		m_assetID = m_assetData->assetQueueID;

		/*assets must have an AssetCategory specified, but may or may not have an AssetSubcategory property.
		if there is an AssetSubcategory property, it is used to determine the correct factory, otherwise
		AssetCategory is used*/
		auto itr = m_assetData->loadParameters.find( L"AssetSubcategory" );
		if( itr != m_assetData->loadParameters.end() )
		{
			assetType = itr->second;
		}

		if( assetType.length() == 0 )
		{
			m_errorMsg = L"Invalid asset: no asset type specified";

		} else
		{
			try
			{
				/*attempt to create a new instance of the factory responsible for creating assets of the desired type*/
				std::unique_ptr<Kiwi::IAssetFactory> factory = _kEngine.CreateFactory<Kiwi::IAssetFactory>( assetType );
				if( factory == nullptr )
				{
					m_errorMsg = L"No suitable asset factory found";

				} else
				{
					//will return a valid pointer to the newly created asset, or will throw an exception
					m_loadedAsset = factory->CreateAsset( assetName, m_assetData->loadParameters );
				}

			} catch( Kiwi::Exception& e )
			{
				m_errorMsg = e.GetError();
			}
		}

		return;

		///*attempt to get the next asset definition from the load queue, and wait for 500 milliseconds
		//for one to be added if one isn't found*/
		//AssetManager::QueuedAssetData* currentAsset = m_assetManager->_FetchNextQueuedAsset();

		//if( currentAsset != nullptr )
		//{
		//	/*an asset definition was returned, proceed to load it*/
		//	std::wstring assetType, assetName;

		//	assetType = currentAsset->assetCategory;
		//	assetName = currentAsset->assetName;

		//	/*assets must have an AssetCategory specified, but may or may not have an AssetSubcategory property.
		//	if there is an AssetSubcategory property, it is used to determine the correct factory, otherwise
		//	AssetCategory is used*/
		//	auto itr = currentAsset->loadParameters.find( L"AssetSubcategory" );
		//	if( itr != currentAsset->loadParameters.end() )
		//	{
		//		assetType = itr->second;

		//	}

		//	if( assetType.length() == 0 )
		//	{
		//		/*notify the asset manager that the asset loading failed*/
		//		m_assetManager->_FinishAsset( currentAsset->assetQueueID, nullptr, L"Invalid asset: no asset type specified" );
		//		return;
		//	}

		//	try
		//	{
		//		/*attempt to create a new instance of the factory responsible for creating assets of the desired type*/
		//		std::unique_ptr<Kiwi::IAssetFactory> factory = _kEngine.CreateFactory<Kiwi::IAssetFactory>( assetType );
		//		if( factory == nullptr )
		//		{
		//			/*notify the asset manager that the asset loading failed*/
		//			m_assetManager->_FinishAsset( currentAsset->assetQueueID, nullptr, L"No suitable asset factory found" );
		//			return;
		//		}

		//		//will return a valid pointer to the newly created asset, or will throw an exception
		//		std::unique_ptr<Kiwi::IAsset> asset = factory->CreateAsset( assetName, currentAsset->loadParameters );

		//		/*give the newly loaded asset back to the asset manager*/
		//		m_assetManager->_FinishAsset( currentAsset->assetQueueID, std::move(asset) );

		//		return;

		//	} catch( Kiwi::Exception& e )
		//	{
		//		/*notify the asset manager that the asset loading failed*/
		//		m_assetManager->_FinishAsset( currentAsset->assetQueueID, nullptr, e.GetError() );
		//		return;
		//	}

		//} else if( m_shutdown == true )
		//{
		//	/*no asset definition exists, if a shutdown has been requested end the thread now*/
		//	m_ready = true;
		//	return;

		//} else
		//{
		//	std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		//}
	}

	void AssetLoadWorker::Finish()
	{
		if( m_loadedAsset == nullptr )
		{
			/*notify the asset manager that the asset loading failed*/
			m_assetManager->_FinishAsset( m_assetID, nullptr, m_errorMsg );

		} else
		{
			/*give the newly loaded asset back to the asset manager*/
			m_assetManager->_FinishAsset( m_assetID, std::move( m_loadedAsset ) );
		}
	}

}