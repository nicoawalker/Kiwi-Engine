#include "AssetManager.h"
#include "Utilities.h"
#include "Exception.h"
#include "IAssetFactory.h"
#include "Scene.h"
#include "EngineRoot.h"
#include "AssetLoadQueue.h"
#include "StaticModelFactory.h"
#include "MaterialFactory.h"

#include "Threading\AssetLoadWorker.h"

#include "../Graphics/StaticModel.h"

namespace Kiwi
{

	AssetManager::AssetManager()
	{
		m_maximumActiveWorkers = 50;
		m_numFailed = m_numLoaded = 0;
		m_loading = false;
		m_currentAssetID = 0;
		m_currentPriority = 0;
	}

	AssetManager::~AssetManager()
	{
		this->_StopAllWorkers();

		/*have to unload assets in reverse load order to avoid invalidating pointers*/
		while( m_managedAssets.size() > 0 )
		{
			AssetGroup* group = nullptr;
			std::unordered_map<std::wstring, std::unique_ptr<AssetGroup>>::iterator groupItr;

			/*get next highest priority asset group*/
			for( auto itr = m_managedAssets.begin(); itr != m_managedAssets.end(); itr++ )
			{
				if( group == nullptr || itr->second->loadPriority > group->loadPriority )
				{
					group = itr->second.get();
					groupItr = itr;
				}
			}

			/*unload the group*/
			Kiwi::FreeMemory( group->assets );

			m_managedAssets.erase( groupItr );
		}
	}

	AssetManager::AssetGroup* AssetManager::_CreateAssetGroup( const std::wstring& assetCategory, unsigned int loadPriority )
	{
		auto groupItr = m_managedAssets.find( assetCategory );
		if( groupItr != m_managedAssets.end() )
		{
			return groupItr->second.get();
		}

		std::unique_ptr<AssetGroup> newGroup = std::make_unique<AssetGroup>( assetCategory );
		if( newGroup == nullptr ) return nullptr;

		newGroup->loadPriority = loadPriority;

		auto insResult = m_managedAssets.insert( std::make_pair( assetCategory, std::move( newGroup ) ) );
		if( insResult.second == false ) return nullptr;

		return insResult.first->second.get();
	}

	void AssetManager::_InitializePrimitiveModels()
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		/*either retrieve the asset group or create a new one*/
		AssetGroup* modelGroup = this->_CreateAssetGroup( L"Model", (unsigned int)LOAD_PRIORITY::MODEL );

		/*create a new factory to create the model*/
		std::unique_ptr<Kiwi::IAssetFactory> modelFactory = _kEngine.CreateFactory<Kiwi::IAssetFactory>( Kiwi::StaticModelFactory::ObjectType );
		if( modelFactory == nullptr )
		{
			_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[AssetManager::_CreatePrimitiveModels] No static model factory exists!" ) );
			return;
		}

		/*use the factory to create the models*/
		std::unique_ptr<Kiwi::IAsset> cubeAsset = modelFactory->CreateAsset( L"Primitive_Cube", Kiwi::StringPair { { L"PrimitiveType", L"cube" } } );
		if( cubeAsset != nullptr )
		{
			Kiwi::StaticModel* model = dynamic_cast<Kiwi::StaticModel*>(cubeAsset.get());
			if( model != nullptr )
			{
				/*create a new, default material and attach it to the model*/
				Kiwi::Material* modelMaterial = this->CreateMaterial( L"Primitive_Cube/Material" );
				if( modelMaterial != nullptr && model->GetSubsetCount() > 0 )
				{
					model->GetSubset( 0 )->SetMaterial( *modelMaterial );
				}

				/*insert the new model into the models group*/
				auto insResult = modelGroup->assets.insert( std::make_pair( L"Primitive_Cube", std::move( cubeAsset ) ) );
				if( insResult.second != false )
				{
					modelGroup->totalAssetCount++;
					KIWIDEBUGSTRING( L"Initialized cube primitive\n" );
				}

			} else
			{
				_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[AssetManager::_CreatePrimitiveModels] Failed to create cube primitive model" ) );
			}

		} else
		{
			_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[AssetManager::_CreatePrimitiveModels] Failed to create cube primitive" ) );
		}

		std::unique_ptr<Kiwi::IAsset> quadAsset = modelFactory->CreateAsset( L"Primitive_Quad", Kiwi::StringPair { { L"PrimitiveType", L"quad" } } );
		if( quadAsset != nullptr )
		{
			Kiwi::StaticModel* model = dynamic_cast<Kiwi::StaticModel*>(quadAsset.get());
			if( model != nullptr )
			{
				/*create a new, default material and attach it to the model*/
				Kiwi::Material* modelMaterial = this->CreateMaterial( L"Primitive_Quad/Material" );
				if( modelMaterial != nullptr && model->GetSubsetCount() > 0 )
				{
					model->GetSubset( 0 )->SetMaterial( *modelMaterial );
				}

				/*insert the new model into the models group*/
				auto insResult = modelGroup->assets.insert( std::make_pair( L"Primitive_Quad", std::move( quadAsset ) ) );
				if( insResult.second != false )
				{
					modelGroup->totalAssetCount++;
				}

			} else
			{
				_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[AssetManager::_CreatePrimitiveModels] Failed to create quad primitive model" ) );
			}

		} else
		{
			_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[AssetManager::_CreatePrimitiveModels] Failed to create quad primitive" ) );
		}
	}

	QueuedAssetData* AssetManager::_FetchNextQueuedAsset()
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		if( m_assetLoadQueue.size() == 0 ) return nullptr;

		/*move the asset from the load queue to the loading queue and return a pointer to it*/
		QueuedAssetData data = *m_assetLoadQueue.begin();
		m_assetLoadQueue.erase( m_assetLoadQueue.begin() );

		auto insResult = m_loadingAssets.insert( std::make_pair( data.assetQueueID, data ) );
		if( insResult.second )
		{
			return &(insResult.first->second);
		}

		return nullptr;
	}

	Kiwi::IAsset* AssetManager::_FindAsset( const std::wstring group, const std::wstring assetName )
	{
		if( group.size() == 0 || assetName.size() == 0 ) return nullptr;

		auto gItr = m_managedAssets.find( group );
		if( gItr != m_managedAssets.end() )
		{
			auto aItr = gItr->second->assets.find( assetName );
			if( aItr != gItr->second->assets.end() )
			{
				return aItr->second.get();
			}
		}

		return nullptr;
	}
	void AssetManager::_FinishAsset( long assetQueueID, std::unique_ptr<Kiwi::IAsset> finishedAsset, const std::wstring& error )
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		auto itr = m_loadingAssets.find( assetQueueID );
		if( itr == m_loadingAssets.end() )
		{
			KIWIDEBUGSTRING( L"Finished loading asset with invalid ID\n" );
		}

		std::wstring assetCategory = itr->second.assetCategory;
		unsigned int loadPriority = itr->second.priority;
		Kiwi::StringPair params = itr->second.loadParameters;

		m_loadingAssets.erase( itr );

		if( finishedAsset == nullptr )
		{
			params.insert( std::make_pair( L"Error", error ) );
			this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( nullptr, L"AssetFailed", params ) );
			return;
		}

		AssetGroup* group = nullptr;

		auto groupItr = m_managedAssets.find( assetCategory );
		if( groupItr == m_managedAssets.end() )
		{
			group = this->_CreateAssetGroup( assetCategory, loadPriority );

		} else
		{
			group = groupItr->second.get();
		}

		if( group == nullptr )
		{
			params.insert( std::make_pair( L"Error", L"No valid asset group exists and one could not be created" ) );
			this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( finishedAsset.get(), L"AssetFailed", params ) );
			return;
		}
		
		auto insResult = group->assets.insert( std::make_pair( finishedAsset->GetAssetName(), std::move( finishedAsset ) ) );
		if( insResult.second == true )
		{
			/*send an asset load event for the new asset*/
			this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( insResult.first->second.get(), L"AssetLoaded", params ) );
			KIWIDEBUGSTRING( Kiwi::ToWString( L"LOADED ASSET " + insResult.first->second->GetAssetName() + L"\n" ).c_str() );

		} else
		{
			group->failCount++;
			params.insert( std::make_pair( L"Error", L"No valid group exists and one could not be created" ) );
			this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( insResult.first->second.get(), L"AssetFailed", params ) );
		}

		if( group->failCount + group->assets.size() >= group->totalAssetCount )
		{
			group->loadStatus = ASSET_GROUP_STATUS::READY;
		}
	}

	void AssetManager::_InsertAsset( std::unique_ptr<Kiwi::IAsset> finishedAsset )
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		if( finishedAsset == nullptr ) return;

		AssetGroup* group = nullptr;

		Kiwi::StringPair params = finishedAsset->GetProperties();

		auto groupItr = m_managedAssets.find( finishedAsset->GetAssetCategory() );
		if( groupItr == m_managedAssets.end() )
		{
			group = this->_CreateAssetGroup( finishedAsset->GetAssetCategory() );

		} else
		{
			group = groupItr->second.get();
		}

		if( group == nullptr )
		{
			params.insert( std::make_pair( L"Error", L"No valid asset group exists and one could not be created" ) );
			this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( finishedAsset.get(), L"AssetFailed", params ) );
			return;
		}

		auto insResult = group->assets.insert( std::make_pair( finishedAsset->GetAssetName(), std::move( finishedAsset ) ) );
		if( insResult.second == true )
		{
			/*send an asset load event for the new asset*/
			this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( insResult.first->second.get(), L"AssetLoaded", params ) );
			KIWIDEBUGSTRING( Kiwi::ToWString( L"LOADED ASSET " + insResult.first->second->GetAssetName() + L"\n" ).c_str() );

		} else
		{
			group->failCount++;
			params.insert( std::make_pair( L"Error", L"No valid asset group exists and one could not be created" ) );
			this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( insResult.first->second.get(), L"AssetFailed", params ) );
		}

		/*assets added this way weren't previously counted so do so now*/
		group->totalAssetCount++;
		if( group->failCount + group->assets.size() >= group->totalAssetCount )
		{
			group->loadStatus = ASSET_GROUP_STATUS::READY;
		}
	}

	void AssetManager::_StopAllWorkers()
	{
		for( std::shared_ptr<Kiwi::AssetLoadWorker>& worker : m_activeWorkers )
		{
			worker->Stop();
		}

		m_activeWorkers.clear();
	}

	void AssetManager::CreateAssetGroup( const std::wstring& assetCategory, unsigned int loadPriority )
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		this->_CreateAssetGroup( assetCategory, loadPriority );
	}

	Kiwi::Material* AssetManager::CreateMaterial( const std::wstring& materialName )
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		/*either retrieve the asset group or create a new one*/
		AssetGroup* matGroup = this->_CreateAssetGroup( L"Material", (unsigned int)LOAD_PRIORITY::MATERIAL );

		/*create a factory to create the material*/
		std::unique_ptr<Kiwi::IAssetFactory> materialFactory = _kEngine.CreateFactory<Kiwi::IAssetFactory>( Kiwi::MaterialFactory::ObjectType );
		if( materialFactory == nullptr )
		{
			_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[AssetManager::CreateMaterial] No material factory exists!" ) );
			return nullptr;
		}

		/*create the material with the factory*/
		std::unique_ptr<Kiwi::IAsset> materialAsset = materialFactory->CreateAsset( materialName );
		if( materialAsset == nullptr ) return nullptr;

		/*insert the material into the materials group*/
		auto insResult = matGroup->assets.insert( std::make_pair( materialName, std::move( materialAsset ) ) );
		if( insResult.second == false ) return nullptr;

		matGroup->totalAssetCount++;

		return dynamic_cast<Material*>(insResult.first->second.get());
	}

	Kiwi::StaticModel* AssetManager::CreateModel( const std::wstring& modelName )
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		/*either retrieve the asset group or create a new one*/
		AssetGroup* modelGroup = this->_CreateAssetGroup( L"Model", (unsigned int)LOAD_PRIORITY::MODEL );
		if( modelGroup == nullptr ) return nullptr;

		/*create a new factory to create the model*/
		std::unique_ptr<Kiwi::IAssetFactory> modelFactory = _kEngine.CreateFactory<Kiwi::IAssetFactory>( Kiwi::StaticModelFactory::ObjectType );
		if( modelFactory == nullptr )
		{
			_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[AssetManager::CreateModel] No static model factory exists!" ) );
			return nullptr;
		}

		/*use the factory to create the model*/
		std::unique_ptr<Kiwi::IAsset> modelAsset = modelFactory->CreateAsset( modelName );
		if( modelAsset == nullptr ) return nullptr;

		/*insert the new model into the models group*/
		auto insResult = modelGroup->assets.insert( std::make_pair( modelName, std::move( modelAsset ) ) );
		if( insResult.second == false ) return nullptr;

		modelGroup->totalAssetCount++;

		return dynamic_cast<Kiwi::StaticModel*>(insResult.first->second.get());
	}

	void AssetManager::Initialize()
	{
		this->_CreateAssetGroup( L"Model", (unsigned int)LOAD_PRIORITY::MODEL );
		this->_CreateAssetGroup( L"Font", (unsigned int)LOAD_PRIORITY::FONT );
		this->_CreateAssetGroup( L"Material", (unsigned int)LOAD_PRIORITY::MATERIAL );
		this->_CreateAssetGroup( L"Shader", (unsigned int)LOAD_PRIORITY::SHADER );
		this->_CreateAssetGroup( L"Texture", (unsigned int)LOAD_PRIORITY::TEXTURE );

		this->_InitializePrimitiveModels();
	}

	void AssetManager::Update()
	{
		if( m_loading == false ) return;

		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		if( m_assetLoadQueue.size() == 0 && m_loadingAssets.size() == 0 )
		{
			m_loading = false;
			this->_StopAllWorkers();
		}

		if( m_activeWorkers.size() < m_maximumActiveWorkers && m_assetLoadQueue.size() > 0 )
		{
			/*if all assets of the current priority level have completed, change the priority
			level to match the first asset in the queue (which is the lowest priority)*/
			if( m_loadingAssets.size() == 0 )
			{
				m_currentPriority = m_assetLoadQueue[0].priority;
			}

			/*start loading all assets with the same priority as the current load priority*/
			for( auto itr = m_assetLoadQueue.begin(); itr != m_assetLoadQueue.end(); )
			{
				if( (*itr).priority > m_currentPriority )break;

				/*need to load lower priority first so if a lower priority asset was added
				while the queue was being loaded we need to load that one first*/
				if( (*itr).priority < m_currentPriority ) m_currentPriority = (*itr).priority;

				std::shared_ptr<Kiwi::AssetLoadWorker> worker = std::make_shared<Kiwi::AssetLoadWorker>( *this, (*itr) );
				if( worker != nullptr )
				{
					_kThreadPool.RunAsync( worker );
					m_activeWorkers.push_back( worker );
					m_loadingAssets.insert( std::make_pair( (*itr).assetQueueID, (*itr) ) );
					itr = m_assetLoadQueue.erase( itr );

				} else
				{
					itr++;
				}
			}
		}
	}

	void AssetManager::EnqueueAsset( const std::wstring& assetCategory, const Kiwi::StringPair& parameters )
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		std::wstring assetName;

		auto paramItr = parameters.find( L"Name" );
		if( paramItr != parameters.end() )
		{
			assetName = paramItr->second;
		}

		if( assetCategory.size() == 0 )
		{
			throw Kiwi::Exception( L"AssetManager::EnqueueAsset", L"Attempted to load asset '" + assetName + L"' without specifying valid category", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		/*make sure the same asset hasn't been created yet*/
		if( assetName.size() > 0 )
		{
			if( this->_FindAsset( assetCategory, assetName ) != nullptr ) return;

			for( const QueuedAssetData& data : m_assetLoadQueue )
			{
				if( data.assetCategory.compare( assetCategory ) == 0 && data.assetName.compare( assetName ) == 0 ) return;
			}
			for( const std::pair<long, QueuedAssetData>& dataPair : m_loadingAssets )
			{
				if( dataPair.second.assetCategory.compare( assetCategory ) == 0 && dataPair.second.assetName.compare( assetName ) == 0 ) return;
			}
		}

		unsigned int priority = UINT_MAX;

		/*create an asset group to match the asset type if one doesn't already exist*/
		auto groupItr = m_managedAssets.find( assetCategory );
		if( groupItr == m_managedAssets.end() )
		{
			//group doesn't exist yet, create it now
			std::unique_ptr<AssetGroup> newGroup = std::make_unique<AssetGroup>( assetCategory );
			newGroup->totalAssetCount = 1;

			/*set the load priority of the new group*/
			if( assetCategory.compare( L"Shader" ) == 0 )
			{
				newGroup->loadPriority = (int)LOAD_PRIORITY::SHADER;
				priority = newGroup->loadPriority;

			} else if( assetCategory.compare( L"Material" ) == 0 )
			{
				newGroup->loadPriority = (int)LOAD_PRIORITY::MATERIAL;
				priority = newGroup->loadPriority;

			} else if( assetCategory.compare( L"Texture" ) == 0 )
			{
				newGroup->loadPriority = (int)LOAD_PRIORITY::TEXTURE;
				priority = newGroup->loadPriority;

			} else if( assetCategory.compare( L"Model" ) == 0 )
			{
				newGroup->loadPriority = (int)LOAD_PRIORITY::MODEL;
				priority = newGroup->loadPriority;

			} else if( assetCategory.compare( L"Font" ) == 0 )
			{
				newGroup->loadPriority = (int)LOAD_PRIORITY::FONT;
				priority = newGroup->loadPriority;

			} else
			{
				auto paramItr = parameters.find( L"Priority" );
				if( paramItr != parameters.end() )
				{
					try
					{
						newGroup->loadPriority = std::stoi( paramItr->second );
						priority = newGroup->loadPriority;

					} catch( std::out_of_range& e )
					{
						KIWIDEBUGSTRING( Kiwi::ToWString( L"Warning loading asset '" + assetName + L"': priority value was too large, defaulting to UINT_MAX\n" ).c_str() );
						newGroup->loadPriority = UINT_MAX;

					} catch( std::exception& e )
					{
						KIWIDEBUGSTRING( Kiwi::ToWString( L"Warning loading asset '" + assetName + L"': invalid priority value, defaulting to UINT_MAX\n" ).c_str() );
						newGroup->loadPriority = UINT_MAX;
					}
				}
			}

			m_managedAssets.insert( std::make_pair( assetCategory, std::move( newGroup ) ) );

		} else
		{
			AssetGroup* group = groupItr->second.get();

			priority = group->loadPriority;
			group->totalAssetCount++;
		}

		m_assetLoadQueue.push_back( QueuedAssetData( m_currentAssetID++, priority, assetCategory, assetName, parameters ) );
		std::sort( m_assetLoadQueue.begin(), m_assetLoadQueue.end(), []( const QueuedAssetData& d1, const QueuedAssetData& d2 )
		{
			return ((d1.assetName + d1.assetCategory).compare( d2.assetName + d2.assetCategory ) != 0) &&
				d1.priority < d2.priority;
		} );
	}

	void AssetManager::Load()
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		if( m_loading || m_assetLoadQueue.size() == 0 ) return;

		m_loading = true;

		for( auto gItr = m_managedAssets.begin(); gItr != m_managedAssets.end(); gItr++ )
		{
			if( gItr->second->totalAssetCount > gItr->second->assets.size() )
			{
				gItr->second->loadStatus = ASSET_GROUP_STATUS::LOADING;
			}
		}
	}

	ASSET_GROUP_STATUS AssetManager::GetGroupLoadStatus( const std::wstring group )
	{
		auto groupItr = m_managedAssets.find( group );
		if( groupItr != m_managedAssets.end() )
		{
			return groupItr->second->loadStatus;
		}

		return ASSET_GROUP_STATUS::INVALID;
	}

	/*destroys the asset with the matching name*/
	void AssetManager::UnloadAsset( const std::wstring group, std::wstring assetName )
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		auto gItr = m_managedAssets.find( group );
		if( gItr != m_managedAssets.end() )
		{
			auto aItr = gItr->second->assets.find( assetName );
			if( aItr != gItr->second->assets.end() )
			{
				if( aItr->second != 0 && aItr->second->GetReferenceCount() != 0 )
				{
					_kEngine.Log( L"Asset '" + aItr->second->GetAssetName() + L"' was destroyed with a non-zero reference count!" );
				}

				this->EnqueueAssetEvent( std::make_shared<Kiwi::AssetEvent>( aItr->second.get(),
																			 L"AssetUnloaded",
																			 aItr->second->GetProperties() ) );
				aItr = gItr->second->assets.erase( aItr );
				gItr->second->totalAssetCount--;
			}
		}
	}

	void AssetManager::UnloadAll()
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		/*unload all assets in reverse order (high to low) based on their priorities*/
		while( m_managedAssets.size() > 0 )
		{
			AssetGroup* group = nullptr;

			/*chose the group with the highest load priority*/
			for( auto gItr = m_managedAssets.begin(); gItr != m_managedAssets.end(); gItr++ )
			{
				if( group == nullptr || gItr->second->loadPriority >= group->loadPriority )
				{
					group = gItr->second.get();
				}
			}

			if( group == nullptr ) break;

			/*unload all the assets in the group*/
			for( auto aItr = group->assets.begin(); aItr != group->assets.end(); )
			{
				if( aItr->second != nullptr )
				{
					if( aItr->second->GetReferenceCount() != 0 )
					{
						_kEngine.Log( L"Asset '" + aItr->second->GetAssetName() + L"' was destroyed with a non-zero reference count!" );
					}

					this->EnqueueAssetEvent( std::make_shared<Kiwi::AssetEvent>( aItr->second.get(),
																				 L"AssetUnloaded",
																				 aItr->second->GetProperties() ) );
				}

				aItr = group->assets.erase( aItr );
			}

			auto gItr = m_managedAssets.find( group->assetCategory );
			m_managedAssets.erase( gItr );
		}
	}

	void AssetManager::Shutdown()
	{
		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
		
		/*unload all assets in reverse order (high to low) based on their priorities*/
		while( m_managedAssets.size() > 0 )
		{
			AssetGroup* group = nullptr;

			/*chose the group with the highest load priority*/
			for( auto gItr = m_managedAssets.begin(); gItr != m_managedAssets.end(); gItr++ )
			{
				if( group == nullptr || gItr->second->loadPriority >= group->loadPriority )
				{
					group = gItr->second.get();
				}
			}

			if( group == nullptr ) break;

			/*unload all the assets in the group*/
			for( auto aItr = group->assets.begin(); aItr != group->assets.end(); )
			{
				if( aItr->second != nullptr && aItr->second->GetReferenceCount() != 0 )
				{
					_kEngine.Log( L"Asset '" + aItr->second->GetAssetName() + L"' was destroyed with a non-zero reference count!" );
				}

				aItr = group->assets.erase( aItr );
			}

			auto gItr = m_managedAssets.find( group->assetCategory );
			m_managedAssets.erase( gItr );
		}
	}

}
























//#include "AssetManager.h"
//#include "Utilities.h"
//#include "Exception.h"
//#include "IAssetFactory.h"
//#include "Scene.h"
//#include "EngineRoot.h"
//#include "AssetLoadQueue.h"
//
//#include "Threading\AssetLoadWorker.h"
//
//namespace Kiwi
//{
//
//	AssetManager::AssetManager()
//	{
//		m_maximumActiveWorkers = 50;
//		m_numFailed = m_numLoaded = 0;
//		m_loading = false;
//		m_currentAssetGroup = nullptr;
//		m_sharedLoadQueue = std::make_shared<Kiwi::AssetLoadQueue>();
//	}
//
//	void AssetManager::_LoadPendingAssetGroups()
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		while( m_assetGroupLoadQueue.size() > 0 && m_assetGroupLoadQueue.front() == nullptr ) m_assetGroupLoadQueue.pop_front();
//
//		/*first check to ensure that no assets are currently loading before starting to load these new
//		ones because assets must be loaded in the correct order*/
//		if( m_assetGroupLoadQueue.size() == 0 || m_loadingAssets.size() != 0 ) return;
//
//		/*get the current lowest priority, then extract all the asset groups with that priority*/
//		unsigned int currentPriority = 0;
//		currentPriority = m_assetGroupLoadQueue.front()->loadPriority;
//
//		std::vector<AssetGroup*> currentGroups;
//		while( m_assetGroupLoadQueue.size() > 0 && m_assetGroupLoadQueue.front()->loadPriority == currentPriority )
//		{
//			currentGroups.push_back( m_assetGroupLoadQueue.front() );
//			m_assetGroupLoadQueue.pop_front();
//		}
//
//		/*determine how many assets there are to be loaded and spawn as many worker threads
//		as are needed, up to the maximum number*/
//		unsigned int assetLoadCount = 0;
//		unsigned int numThreads = m_maximumActiveWorkers - (unsigned int)m_activeWorkers.size();
//		if( m_activeWorkers.size() <= m_maximumActiveWorkers )
//		{
//			for( AssetGroup* group : currentGroups )
//			{
//				auto itr = m_unloadedAssets.find( group->assetCategory );
//				if( itr != m_unloadedAssets.end() )
//				{
//					assetLoadCount += (unsigned int)itr->second.size();
//				}
//			}
//		}
//
//		/*determine how many threads to spawn. num threads is the current maximum number of spawnable threads*/
//		numThreads = (assetLoadCount < numThreads) ? assetLoadCount : numThreads;
//
//		this->_LaunchWorkerThreads( numThreads );
//
//		/*add every asset definition from all the groups to the shared load queue and the m_loadingAssets set*/
//		for( AssetGroup* group : currentGroups )
//		{
//			auto itr = m_unloadedAssets.find( group->assetCategory );
//			if( itr != m_unloadedAssets.end() )
//			{
//				for( Kiwi::StringPair& assetDef : itr->second )
//				{
//					//retrieve the name of the asset
//					std::wstring assetName;
//					auto nameItr = assetDef.find( L"Name" );
//					if( nameItr != assetDef.end() )
//					{
//						m_loadingAssets.insert( nameItr->second );
//						m_sharedLoadQueue->Enqueue( assetDef );
//
//					} else
//					{
//						//if the asset has no name it cannot be loaded so skip it and output a debug message
//						_kConsole.Print( L"SystemDebug", L"Attempted to load asset without a name" );
//						KIWIDEBUGSTRING( L"Attempted to load asset without a name\n" );
//					}
//				}
//
//				//assets are being loaded so remove them from the unloaded list
//				m_unloadedAssets.erase( itr );
//			}
//		}
//
//		m_loading = true;
//
//		/*if( m_assetGroupLoadQueue.size() == 0 ) return;
//
//		AssetGroup* group = m_assetGroupLoadQueue.front();
//
//		if( group != 0 && group->loadStatus != ASSET_GROUP_STATUS::LOADING )
//		{
//		while(group->assetQueue.size() > 0 )
//		{
//		group->loadStatus = ASSET_GROUP_STATUS::LOADING;
//		_kThreadPool.RunAsync( group->assetQueue.front() );
//		group->assetQueue.pop_front();
//		}
//		}*/
//	}
//
//	void AssetManager::_AddLoadedAsset( const Kiwi::StringPair& assetDefinition, std::unique_ptr<Kiwi::IAsset> loadedAsset )
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		/*add the new asset to the loaded asset buffer so that an asset load event will be sent
//		during the next update cycle*/
//		m_loadedAssetBuffer.push_back( AssetDefinitionPair( std::move( loadedAsset ), assetDefinition ) );
//
//		//if( existing != nullptr )
//		//{
//		//	std::wstring assetType = existing->GetAssetType();
//		//	std::wstring assetName = existing->GetAssetName();
//		//	Kiwi::IAsset* asset = existing.get();
//
//		//	auto groupItr = m_assetGroups.find( group );
//		//	if( groupItr != m_assetGroups.end() )
//		//	{
//		//		//found the group, add the asset to the group
//		//		groupItr->second->assets.insert( std::make_pair( existing->GetAssetName(), std::move( existing ) ) );
//		//		groupItr->second->assetCount++;
//
//		//		if( groupItr->second->assetCount == groupItr->second->totalAssetCount )
//		//		{
//		//			groupItr->second->loadStatus = ASSET_GROUP_STATUS::READY;
//		//		} 
//
//		//		this->EnqueueAssetEvent( std::make_shared<Kiwi::AssetEvent>( asset,
//		//																	 L"AssetLoaded",
//		//																	 Kiwi::MakeType::StringPair( {
//		//																		 { L"Name", assetName },
//		//																		 { L"AssetType", assetType },
//		//																		 { L"Group", group } } ) ) );
//
//		//	}
//		//}
//	}
//
//	void AssetManager::_FinishLoadedAssets()
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		for( AssetDefinitionPair& adPair : m_loadedAssetBuffer )
//		{
//			if( adPair.asset != nullptr )
//			{//asset loading was successful
//
//			 /*remove the asset from the list of loading assets*/
//				m_loadingAssets.erase( adPair.asset->GetAssetName() );
//
//				KIWIDEBUGSTRING( Kiwi::ToWString( L"Asset loaded: " + adPair.asset->GetAssetName() + L"\n" ).c_str() );
//
//				auto groupItr = m_assetGroups.find( adPair.asset->GetAssetCategory() );
//				if( groupItr != m_assetGroups.end() )
//				{
//					AssetGroup* group = groupItr->second.get();
//					if( group != nullptr )
//					{
//						/*insert the asset into the correct group*/
//						bool inserted = group->assets.insert( std::make_pair( adPair.asset->GetAssetName(), std::move( adPair.asset ) ) ).second;
//						if( inserted == false )
//						{
//							KIWIDEBUGSTRING( Kiwi::ToWString( L"Failed to insert new asset \"" + adPair.asset->GetAssetName() + L"\" into asset group \"" + adPair.asset->GetAssetCategory() + L"\"\n" ).c_str() );
//							continue;
//						}
//
//						/*send an asset load event for the new asset*/
//						this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( adPair.asset.get(), L"AssetLoaded", adPair.definition ) );
//
//						/*send an event if all assets in the group have finished loading*/
//						if( group->assets.size() + group->failCount == group->totalAssetCount )
//						{
//							group->loadStatus = ASSET_GROUP_STATUS::READY;
//						}
//					}
//				}
//
//			} else
//			{//asset loading failed
//
//			 /*extract the reason for the load failure*/
//				std::wstring assetName;
//				auto nameItr = adPair.definition.find( L"Name" );
//				if( nameItr != adPair.definition.end() )
//				{
//					assetName = nameItr->second;
//
//					/*remove the asset from the list of loading assets*/
//					m_loadingAssets.erase( assetName );
//
//				} else
//				{
//					//loaded an asset with no name
//					_kConsole.Print( L"SystemDebug", L"Loaded asset without a name" );
//					KIWIDEBUGSTRING( L"Loaded asset without a name\n" );
//				}
//
//				std::wstring failString;
//				auto failItr = adPair.definition.find( L"FailMessage" );
//				if( failItr != adPair.definition.end() )
//				{
//					failString = failItr->second;
//					KIWIDEBUGSTRING( Kiwi::ToWString( L"Asset '" + assetName + L"' failed to load: " + failString + L"\n" ).c_str() );
//
//				} else
//				{
//					KIWIDEBUGSTRING( Kiwi::ToWString( L"Asset '" + assetName + L"' failed to load for an unspecified reason\n" ).c_str() );
//				}
//
//				/*increase the fail count for the group the asset would have belonged to*/
//				std::wstring assetType;
//				auto typeItr = adPair.definition.find( L"AssetCategory" );
//				if( typeItr != adPair.definition.end() )
//				{
//					assetType = typeItr->second;
//					auto groupItr = m_assetGroups.find( assetType );
//					if( groupItr != m_assetGroups.end() )
//					{
//						AssetGroup* group = groupItr->second.get();
//						if( group != nullptr )
//						{
//							group->failCount++;
//
//							/*send an event if all assets in the group have finished loading*/
//							if( group->assets.size() + group->failCount == group->totalAssetCount )
//							{
//								group->loadStatus = ASSET_GROUP_STATUS::READY;
//							}
//						}
//					}
//
//				} else
//				{
//					//loaded an asset with no type
//					_kConsole.Print( L"SystemDebug", L"Loaded asset without a type" );
//					KIWIDEBUGSTRING( L"Loaded asset without a type\n" );
//				}
//
//				/*send an asset load event for the new asset*/
//				this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( nullptr, L"AssetFailed", adPair.definition ) );
//			}
//		}
//
//		m_loadedAssetBuffer.clear();
//
//		/*KIWIDEBUGSTRING( Kiwi::ToWString( L"Asset load fail: " + exception.GetError() + L"\n" ).c_str() );
//
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		auto groupItr = m_assetGroups.find( group );
//		if( groupItr != m_assetGroups.end() )
//		{
//		groupItr->second->totalAssetCount--;
//		}
//
//		_kConsole.Print( L"Warning", L"Failed to load asset '" + assetName + L"' of type '" + assetType + L"':" );
//		_kConsole.PrintException( exception );
//		_kConsole.Print( L"Warning", L"Asset Parameters:" );
//		for( auto itr = parameters.begin(); itr != parameters.end(); itr++ )
//		{
//		std::wstring line = itr->first + L": " + itr->second;
//		_kConsole.Print( L"Warning", line );
//		}
//
//		_kEngine.Log( L"Exception: " + exception.GetSource() + L", " + exception.GetError() );
//
//		this->EnqueueAssetEvent ( std::make_shared<Kiwi::AssetEvent>( nullptr,
//		L"AssetFailed",
//		Kiwi::MakeType::StringPair( {
//		{ L"Name", assetName },
//		{ L"AssetType", assetType },
//		{ L"Group", group } } ) ) );*/
//
//	}
//
//	Kiwi::IAsset* AssetManager::_FindAsset( const std::wstring group, const std::wstring assetName )
//	{
//		auto gItr = m_assetGroups.find( group );
//		if( gItr != m_assetGroups.end() )
//		{
//			auto aItr = gItr->second->assets.find( assetName );
//			if( aItr != gItr->second->assets.end() )
//			{
//				return aItr->second.get();
//			}
//		}
//
//		return nullptr;
//	}
//
//	void AssetManager::_LaunchWorkerThreads( unsigned int threadCount )
//	{
//		for( unsigned int i = 0; i < threadCount; i++ )
//		{
//			std::shared_ptr<Kiwi::AssetLoadWorker> worker = std::make_shared<Kiwi::AssetLoadWorker>( *this );
//			if( worker != nullptr )
//			{
//				_kThreadPool.RunAsync( worker );
//				m_activeWorkers.push_back( worker );
//			}
//		}
//	}
//
//	void AssetManager::_ShutdownAllWorkerThreads()
//	{
//		for( std::shared_ptr<Kiwi::AssetLoadWorker>& worker : m_activeWorkers )
//		{
//			worker->Stop();
//		}
//
//		m_activeWorkers.clear();
//	}
//
//	void AssetManager::Update()
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		if( m_loadedAssetBuffer.size() > 0 ) this->_FinishLoadedAssets();
//
//		if( m_assetGroupLoadQueue.size() > 0 )
//		{
//			this->_LoadPendingAssetGroups();
//
//		} else if( m_activeWorkers.size() != 0 )
//		{
//			this->_ShutdownAllWorkerThreads();
//		}
//
//		/*if currently loading, check whether all asset groups have finished, then send a LoadFinished event*/
//		if( m_loading )
//		{
//			unsigned int loadCount = 0;
//			for( auto itr = m_assetGroups.begin(); itr != m_assetGroups.end(); itr++ )
//			{
//				if( itr->second->loadStatus == ASSET_GROUP_STATUS::READY ) loadCount++;
//			}
//
//			if( loadCount == (unsigned int)m_assetGroups.size() )
//			{
//				m_loading = false;
//
//				this->EmitAssetEvent( std::make_shared<Kiwi::AssetEvent>( nullptr, L"LoadFinished", Kiwi::MakeType::StringPair( { { L"AssetCount", Kiwi::ToWString( m_numFailed + m_numLoaded ) },
//				{ L"NumFailed", Kiwi::ToWString( m_numFailed ) } } ) ) );
//			}
//		}
//	}
//
//	void AssetManager::EnqueueAsset( const std::wstring& assetCategory, const Kiwi::StringPair& parameters )
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		std::wstring assetName;
//
//		auto paramItr = parameters.find( L"Name" );
//		if( paramItr != parameters.end() )
//		{
//			assetName = paramItr->second;
//
//		} else
//		{
//			throw Kiwi::Exception( L"AssetManager::EnqueueAsset", L"Attempted to load an asset without a name", KIWI_EXCEPTION::INVALIDPARAMETER );
//		}
//
//		if( assetCategory.size() == 0 )
//		{
//			throw Kiwi::Exception( L"AssetManager::EnqueueAsset", L"Attempted to load asset '" + assetName + L"' without specifying valid category", KIWI_EXCEPTION::INVALIDPARAMETER );
//		}
//
//		/*create an asset group to match the asset type if one doesn't already exist*/
//		auto groupItr = m_assetGroups.find( assetCategory );
//		if( groupItr == m_assetGroups.end() )
//		{
//			//group doesn't exist yet, create it now
//			std::unique_ptr<AssetGroup> newGroup = std::make_unique<AssetGroup>( assetCategory );
//			newGroup->totalAssetCount = 1;
//
//			/*set the load priority of the new group*/
//			if( assetCategory.compare( L"Shader" ) == 0 )
//			{
//				newGroup->loadPriority = (int)LOAD_PRIORITY::SHADER;
//
//			} else if( assetCategory.compare( L"Texture" ) == 0 )
//			{
//				newGroup->loadPriority = (int)LOAD_PRIORITY::TEXTURE;
//
//			} else if( assetCategory.compare( L"Model" ) == 0 )
//			{
//				newGroup->loadPriority = (int)LOAD_PRIORITY::MODEL;
//
//			} else if( assetCategory.compare( L"Font" ) == 0 )
//			{
//				newGroup->loadPriority = (int)LOAD_PRIORITY::FONT;
//
//			} else
//			{
//				auto paramItr = parameters.find( L"Priority" );
//				if( paramItr != parameters.end() )
//				{
//					try
//					{
//						newGroup->loadPriority = std::stoi( paramItr->second );
//
//					} catch( std::out_of_range& e )
//					{
//						KIWIDEBUGSTRING( Kiwi::ToWString( L"Warning loading asset '" + assetName + L"': priority value was too large, defaulting to UINT_MAX\n" ).c_str() );
//						newGroup->loadPriority = UINT_MAX;
//
//					} catch( std::exception& e )
//					{
//						KIWIDEBUGSTRING( Kiwi::ToWString( L"Warning loading asset '" + assetName + L"': invalid priority value, defaulting to UINT_MAX\n" ).c_str() );
//						newGroup->loadPriority = UINT_MAX;
//					}
//				}
//			}
//
//			m_assetGroups.insert( std::make_pair( assetCategory, std::move( newGroup ) ) );
//		}
//
//		auto assetItr = m_unloadedAssets.find( assetCategory );
//		if( assetItr != m_unloadedAssets.end() )
//		{
//			assetItr->second.push_back( parameters );
//
//		} else
//		{
//			std::vector<Kiwi::StringPair> temp = { parameters };
//			m_unloadedAssets.insert( std::make_pair( assetCategory, temp ) );
//		}
//	}
//
//	void AssetManager::LoadAssetGroup( const std::wstring group )
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		auto groupItr = m_assetGroups.find( group );
//		if( groupItr != m_assetGroups.end() )
//		{
//			groupItr->second->loadStatus = ASSET_GROUP_STATUS::LOADING;
//
//			m_assetGroupLoadQueue.push_back( groupItr->second.get() );
//
//			/*sort the queue by priority*/
//			std::sort( m_assetGroupLoadQueue.begin(),
//					   m_assetGroupLoadQueue.end(),
//					   []( const AssetGroup* lhs, const AssetGroup* rhs )->bool{
//				return lhs->loadPriority < rhs->loadPriority;
//			} );
//		}
//	}
//
//	void AssetManager::Load()
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		for( auto gItr = m_assetGroups.begin(); gItr != m_assetGroups.end(); gItr++ )
//		{
//			gItr->second->loadStatus = ASSET_GROUP_STATUS::LOADING;
//			m_assetGroupLoadQueue.push_back( gItr->second.get() );
//		}
//
//		/*sort the queue by load priority*/
//		std::sort( m_assetGroupLoadQueue.begin(),
//				   m_assetGroupLoadQueue.end(),
//				   []( const AssetGroup* lhs, const AssetGroup* rhs )->bool{
//			return lhs->loadPriority < rhs->loadPriority;
//		} );
//	}
//
//	ASSET_GROUP_STATUS AssetManager::GetGroupLoadStatus( const std::wstring group )
//	{
//		auto groupItr = m_assetGroups.find( group );
//		if( groupItr != m_assetGroups.end() )
//		{
//			return groupItr->second->loadStatus;
//		}
//
//		return ASSET_GROUP_STATUS::INVALID;
//	}
//
//	/*destroys the asset with the matching name*/
//	void AssetManager::UnloadAsset( const std::wstring group, std::wstring assetName )
//	{
//		std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		auto gItr = m_assetGroups.find( group );
//		if( gItr != m_assetGroups.end() )
//		{
//			auto aItr = gItr->second->assets.find( assetName );
//			if( aItr != gItr->second->assets.end() )
//			{
//				if( aItr->second != 0 && aItr->second->GetReferenceCount() != 0 )
//				{
//					_kEngine.Log( L"Asset '" + aItr->second->GetAssetName() + L"' was destroyed with a non-zero reference count!" );
//				}
//
//				this->EnqueueAssetEvent( std::make_shared<Kiwi::AssetEvent>( aItr->second.get(),
//																			 L"AssetUnloaded",
//																			 aItr->second->GetProperties() ) );
//				aItr = gItr->second->assets.erase( aItr );
//				gItr->second->totalAssetCount--;
//			}
//		}
//
//	}
//
//}