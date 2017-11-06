#ifndef _KIWI_ASSETMANAGER_H_
#define _KIWI_ASSETMANAGER_H_

/*this class is a manager for raw asset objects loaded from disk*/

#include "IAsset.h"
#include "ThreadParam.h"
#include "IAssetFactory.h"

#include "..\Types.h"

#include "../Events/IAssetEventEmitter.h"
#include "../Events/Signal.h"

#include "..\Graphics\IModel.h"

#include <unordered_map>
#include <deque>
#include <vector>
#include <set>
#include <string>
#include <memory>
#include <mutex>

namespace Kiwi
{

	class Scene;
	class StaticModel;
	class AssetLoadWorker;
	class AssetLoadQueue;

	enum AssetMessage { Loaded, Unloaded, Finished };

	enum class ASSET_GROUP_STATUS { UNREADY = 0, LOADING = 1, READY = 2, INVALID = 1000 };

	struct QueuedAssetData
	{
		long assetQueueID;
		unsigned int priority;
		std::wstring assetCategory;
		std::wstring assetName;
		Kiwi::StringPair loadParameters;

		QueuedAssetData( long id, unsigned int priority, const std::wstring& category, const std::wstring& name, const Kiwi::StringPair& params )
		{
			assetQueueID = id;
			this->priority = priority;
			assetCategory = category;
			assetName = name;
			loadParameters = params;
		}

		QueuedAssetData( const QueuedAssetData& data )
		{
			assetQueueID = data.assetQueueID;
			priority = data.priority;
			assetCategory = data.assetCategory;
			assetName = data.assetName;
			loadParameters = data.loadParameters;
		}
	};

	class AssetManager:
		public Kiwi::IAssetEventEmitter
	{
		friend class AssetLoadWorker;
		friend class IAssetFactory;
	private:

		enum class LOAD_PRIORITY { SHADER = 0, MATERIAL, TEXTURE, MODEL, FONT };

		struct AssetGroup
		{
			unsigned int failCount; //number of assets from this group that failed to load
			unsigned int totalAssetCount; //total number of assets that should belong to the group (including failed assets)
			unsigned int loadPriority; //priority of the group when loading, groups load in order from highest priority (0) to lowest
			ASSET_GROUP_STATUS loadStatus;
			std::wstring assetCategory; //category of asset the group contains
			std::unordered_map<std::wstring, std::unique_ptr<Kiwi::IAsset>> assets; //map of all loaded assets in this group

			AssetGroup( std::wstring assetCategory )
			{
				this->assetCategory = assetCategory;
				loadStatus = ASSET_GROUP_STATUS::UNREADY;
				failCount = 0;
				totalAssetCount = 0;
			}
		};

		struct AssetDefinitionPair
		{
			std::unique_ptr<Kiwi::IAsset> asset;
			Kiwi::StringPair definition;

			AssetDefinitionPair( std::unique_ptr<Kiwi::IAsset> ass, const Kiwi::StringPair& def )
			{
				asset = std::move( ass );
				definition = def;
			}
		};

	protected:

		/*contains all asset groups, with the key being the asset type associated to each group*/
		std::unordered_map<std::wstring, std::unique_ptr<AssetGroup>> m_managedAssets;

		/*stores data on assets that are to be loaded*/
		std::vector<QueuedAssetData> m_assetLoadQueue;

		/*stores data on assets that are being loaded. the key is the assetQueueID*/
		std::unordered_map<long, QueuedAssetData> m_loadingAssets;

		/*contains all asset groups, with the key being the asset type associated to each group*/
		//std::unordered_map<std::wstring, std::unique_ptr<AssetGroup>> m_assetGroups;

		/*map containing information about all assets that have not yet been loaded
		key is the asset type (group) of the asset, and the value is a vector of all assets of that type*/
		//std::unordered_map<std::wstring, std::vector<Kiwi::StringPair>> m_unloadedAssets;

		/*names of all the assets that are currently being loaded, when an asset loads or fails
		to load it is removed from the list*/
		//std::unordered_set<std::wstring> m_loadingAssets;

		/*load queue shared with the worker threads*/
		//std::shared_ptr<Kiwi::AssetLoadQueue> m_sharedLoadQueue;

		/*contains each asset group that is queued to be loaded, sorted by group load priority*/
		//std::deque<AssetGroup*> m_assetGroupLoadQueue;

		/*list of asset workers that are currently running in threads*/
		std::vector<std::shared_ptr<AssetLoadWorker>> m_activeWorkers;

		/*buffer containing loaded (or failed) assets returned by the worker threads which have
		not yet been added to the correct asset group*/
		//std::vector<AssetDefinitionPair> m_loadedAssetBuffer;

		std::recursive_mutex m_assetManager_M;

		/*pointer to the asset group currently being loaded*/
		//AssetGroup* m_currentAssetGroup;

		/*maximum number of active workers that will be running at any given time*/
		unsigned int m_maximumActiveWorkers;

		unsigned int m_numLoaded, m_numFailed;

		/*unique id given to the next enqueued asset*/
		unsigned int m_currentAssetID;

		/*set to true to indicate that the asset manager is currently loading assets*/
		bool m_loading;

		/*stores the priority level of the assets that are currently being loaded*/
		unsigned int m_currentPriority;

	public:

		/*connect to this signal to receive a notification when an asset finishes loading*/
		Kiwi::Signal<void( Kiwi::IAsset* )> onAssetLoaded;

		/*connect to this signal to receive a notification when an asset is about to be unloaded*/
		Kiwi::Signal<void( Kiwi::IAsset* )> onAssetUnloaded;

		/*connect to this signal to receive a notification when an asset fails to load*/
		Kiwi::Signal<void( std::wstring )> onAssetFailed;

	protected:

		AssetGroup* _CreateAssetGroup( const std::wstring& assetCategory, unsigned int loadPriority = UINT_MAX );

		/*creates primitive models that can be easily used*/
		void _InitializePrimitiveModels();

		Kiwi::IAsset* _FindAsset( const std::wstring assetCategory, const std::wstring assetName );

		/*called by the worker threads to fetch the next asset to be loaded*/
		AssetManager::QueuedAssetData* _FetchNextQueuedAsset();

		/*called by the worker threads to give a finished asset back to the asset manager*/
		void _FinishAsset( long assetQueueID, std::unique_ptr<Kiwi::IAsset> finishedAsset, const std::wstring& error = L"" );

		/*inserts a loaded asset directly into the managed asset list. used when an asset factory
		creates more than one asset at a time*/
		void _InsertAsset( std::unique_ptr<Kiwi::IAsset> finishedAsset );

		/*notifies all active worker threads to shutdown immediately*/
		void _StopAllWorkers();

	public:

		AssetManager();
		~AssetManager();

		void CreateAssetGroup( const std::wstring& assetCategory, unsigned int loadPriority = UINT_MAX );

		/*creates a new empty material*/
		Kiwi::Material* CreateMaterial( const std::wstring& materialName );

		/*creates and returns a pointer to a new empty model*/
		Kiwi::StaticModel* CreateModel( const std::wstring& modelName );

		void Initialize();

		void Update();

		/*adds an asset to a group that will be loaded when the group is loaded
		|-assetCategeory: type of asset to create. this also functions as the name of the group. if a 
		|	matching group doesn't exist a new one will be created
		|-parameters: parameters needed to initialize the asset*/
		void EnqueueAsset( const std::wstring& assetCategory, const Kiwi::StringPair& parameters );

		/*begins loading of all currently unloaded asset groups*/
		void Load();

		ASSET_GROUP_STATUS GetGroupLoadStatus( const std::wstring group );

		/*unloads the managed asset with the matching name and category/subcategory*/
		void UnloadAsset( const std::wstring assetCategory, const std::wstring assetName );

		void UnloadAll();

		/*searches the current assets and returns the one with the matching name, or nullptr is a match isnt found*/
		template<class AssetType>
		AssetType* FindAsset( const std::wstring assetCategory, const std::wstring assetName )
		{
			std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

			return dynamic_cast<AssetType*>(this->_FindAsset( assetCategory, assetName ));
		}

		void Shutdown();

		/*creates a copy of a model that is currently in the manager as a shared model. the shared model will be
		identical to the original except that it will also have an instancebuffer that enables it to be efficiently
		shared between many meshes simultaneously. if an existing shared model with the same name exists, or the
		source model doesn't exist, a new shared model will not be created and the function will return nullptr.
		|-ModelType: type of model that is to be copied and created
		|-sharedModelName: unique name of the new shared model
		|-sourceModel: name of the existing model that is to be copied*/
		//template<typename ModelType>
		//Kiwi::IModel* CreateSharedModel( const std::wstring& sharedModelName, const std::wstring& sourceModel )
		//{
		//	std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

		//	//check for any existing shared model with the same name
		//	Kiwi::IModel* existingShared = dynamic_cast<Kiwi::IModel*>(this->_FindAsset( L"Model", sharedModelName ));
		//	if( existingShared != nullptr ) return existingShared;

		//	//attempt to find the source model for the copy
		//	ModelType* existingModel = dynamic_cast<ModelType*>(this->_FindAsset( L"Model", sourceModel ));
		//	if( existingModel == nullptr ) return nullptr;

		//	//create a new copy of the source model
		//	std::unique_ptr<ModelType> newModel = std::make_unique<ModelType>( existingModel );
		//	if( newModel == nullptr ) return nullptr;

		//	Kiwi::IModel* sharedModel = newModel.get();
		//	sharedModel->SetShared( true );
		//	sharedModel->SetAssetName( sharedModelName );

		//	//insert the new model into the same group as the original
		//	auto gItr = m_assetGroups.find( sharedModel->GetAssetCategory() );
		//	if( gItr != m_assetGroups.end() )
		//	{
		//		bool inserted = gItr->second->assets.insert( std::make_pair( sharedModelName, std::move( newModel ) ) ).second;
		//		if( inserted == false )
		//		{
		//			KIWIDEBUGSTRING( Kiwi::ToWString( L"Failed to insert new shared model \"" + sharedModelName + L"\" into asset group \"Model\"\n" ).c_str() );
		//			return nullptr;
		//		}
		//	}

		//	return sharedModel;
		//}

		template<typename AssetType>
		AssetType* CopyAsset( const std::wstring& assetCategory, const std::wstring& sourceAssetName, const std::wstring& newAssetName )
		{
			std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );

			//get the existing asset
			Kiwi::IAsset* asset = this->_FindAsset( assetCategory, sourceAssetName );
			if( asset == nullptr ) return nullptr;

			//clone the existing asset
			std::unique_ptr<Kiwi::IAsset> newAsset = asset->Clone();
			if( newAsset == nullptr ) return nullptr;

			newAsset->SetAssetName( newAssetName );

			//insert the new asset into the same group as the original
			auto gItr = m_managedAssets.find( assetCategory );
			if( gItr != m_managedAssets.end() )
			{
				auto insVal = gItr->second->assets.insert( std::make_pair( newAssetName, std::move( newAsset ) ) );
				if( insVal.second == false )
				{
					KIWIDEBUGSTRING( Kiwi::ToWString( L"Failed to copy asset \"" + sourceAssetName + L"\" as \"" + newAssetName + L"\"\n" ).c_str() );
					return nullptr;
				}

				return dynamic_cast<AssetType*>(insVal.first->second.get());

			}
			return nullptr;
		}

	};
}

#endif
















//#ifndef _KIWI_ASSETMANAGER_H_
//#define _KIWI_ASSETMANAGER_H_
//
///*this class is a manager for raw asset objects loaded from disk*/
//
//#include "IAsset.h"
//#include "ThreadParam.h"
//
//#include "..\Types.h"
//
//#include "../Events/IAssetEventEmitter.h"
//
//#include "..\Graphics\IModel.h"
//
//#include <unordered_map>
//#include <deque>
//#include <vector>
//#include <set>
//#include <string>
//#include <memory>
//#include <mutex>
//
//namespace Kiwi
//{
//
//	class Scene;
//	class AssetLoadWorker;
//	class AssetLoadQueue;
//
//	enum class ASSET_GROUP_STATUS { UNREADY = 0, LOADING = 1, READY = 2, INVALID = 1000 };
//
//	class AssetManager :
//		public Kiwi::IAssetEventEmitter
//	{
//		friend class AssetLoadWorker;
//	private:
//
//		enum class LOAD_PRIORITY { SHADER = 0, TEXTURE, MODEL, FONT };
//
//		struct AssetGroup
//		{
//			unsigned int failCount; //number of assets from this group that failed to load
//			unsigned int totalAssetCount; //total number of assets that should belong to the group (including failed assets)
//			unsigned int loadPriority; //priority of the group when loading, groups load in order from highest priority (0) to lowest
//			ASSET_GROUP_STATUS loadStatus;
//			std::wstring assetCategory; //category of asset the group contains
//			std::unordered_map<std::wstring, std::unique_ptr<Kiwi::IAsset>> assets; //map of all loaded assets in this group
//
//			AssetGroup( std::wstring assetCategory )
//			{
//				this->assetCategory = assetCategory;
//				loadStatus = ASSET_GROUP_STATUS::UNREADY;
//				failCount = 0;
//				totalAssetCount = 0;
//			}
//		};
//
//		struct QueuedAssetData
//		{
//			unsigned int priority;
//			std::wstring assetCategory;
//			std::wstring assetName;
//			Kiwi::StringPair loadParameters;
//
//			QueuedAssetData( unsigned int priority, const std::wstring& category, const std::wstring& name, const Kiwi::StringPair& params )
//			{
//				this->priority = priority;
//				assetCategory = category;
//				assetName = name;
//				loadParameters = params;
//			}
//		};
//
//		struct QueuedAssetDataHasher
//		{
//			size_t operator()( const QueuedAssetData& d1 )const
//			{
//				return std::hash<std::wstring>()(d1.assetName + d1.assetCategory);
//			}
//		};
//
//		struct QueuedAssetDataComparator
//		{
//			bool operator()( const QueuedAssetData& d1, const QueuedAssetData& d2 )const
//			{
//				return d1.priority < d2.priority;
//			}
//		};
//
//		struct AssetDefinitionPair
//		{
//			std::unique_ptr<Kiwi::IAsset> asset;
//			Kiwi::StringPair definition;
//
//			AssetDefinitionPair( std::unique_ptr<Kiwi::IAsset> ass, const Kiwi::StringPair& def )
//			{
//				asset = std::move( ass );
//				definition = def;
//			}
//		};
//
//	protected:
//
//		/*contains all asset groups, with the key being the asset type associated to each group*/
//		std::unordered_map<std::wstring, std::unique_ptr<AssetGroup>> m_managedAssets;
//
//		/*stores data on assets that are to be loaded*/
//		std::set<QueuedAssetData, QueuedAssetDataHasher, QueuedAssetDataComparator> m_assetLoadQueue;
//
//		/*stores data on assets that are being loaded*/
//		std::unordered_map<std::wstring, QueuedAssetData> m_loadingAssets;
//
//
//
//		/*contains all asset groups, with the key being the asset type associated to each group*/
//		//std::unordered_map<std::wstring, std::unique_ptr<AssetGroup>> m_assetGroups;
//
//		/*map containing information about all assets that have not yet been loaded
//		key is the asset type (group) of the asset, and the value is a vector of all assets of that type*/
//		//std::unordered_map<std::wstring, std::vector<Kiwi::StringPair>> m_unloadedAssets;
//
//		/*names of all the assets that are currently being loaded, when an asset loads or fails
//		to load it is removed from the list*/
//		//std::unordered_set<std::wstring> m_loadingAssets;
//
//		/*load queue shared with the worker threads*/
//		//std::shared_ptr<Kiwi::AssetLoadQueue> m_sharedLoadQueue;
//
//		/*contains each asset group that is queued to be loaded, sorted by group load priority*/
//		//std::deque<AssetGroup*> m_assetGroupLoadQueue;
//
//		/*list of asset workers that are currently running in threads*/
//		std::vector<std::shared_ptr<AssetLoadWorker>> m_activeWorkers;
//
//		/*buffer containing loaded (or failed) assets returned by the worker threads which have
//		not yet been added to the correct asset group*/
//		//std::vector<AssetDefinitionPair> m_loadedAssetBuffer;
//
//		std::recursive_mutex m_assetManager_M;
//
//		/*pointer to the asset group currently being loaded*/
//		//AssetGroup* m_currentAssetGroup;
//
//		/*maximum number of active workers that will be running at any given time*/
//		unsigned int m_maximumActiveWorkers;
//
//		unsigned int m_numLoaded, m_numFailed;
//
//		/*set to true to indicate that the asset manager is currently loading assets*/
//		bool m_loading;
//
//	protected:
//
//		/*called by AssetLoadWorkers to add their loaded assets to the asset manager upon load completion*/
//		void _AddLoadedAsset( const Kiwi::StringPair& assetDefinition, std::unique_ptr<Kiwi::IAsset> loadedAsset );
//
//		Kiwi::IAsset* _FindAsset( const std::wstring assetCategory, const std::wstring assetName );
//
//		/*for each asset in the loaded asset buffer, the asset is added to its respective group (if it loaded successfully)
//		and then a load/failure event is sent to the receivers*/
//		void _FinishLoadedAssets();
//
//		/*starts loading all assets from the next group(s) with the highest priority*/
//		void _LoadPendingAssetGroups();
//
//		/*launches a specified number of worker threads*/
//		void _LaunchWorkerThreads( unsigned int threadCount );
//
//		/*notifies all active worker threads to shutdown immediately*/
//		void _ShutdownAllWorkerThreads();
//
//	public:
//
//		AssetManager();
//		~AssetManager() {}
//
//		void Update();
//
//		/*adds an asset to a group that will be loaded when the group is loaded
//		|-assetCategeory: type of asset to create. this also functions as the name of the group. if a
//		|	matching group doesn't exist a new one will be created
//		|-parameters: parameters needed to initialize the asset*/
//		void EnqueueAsset( const std::wstring& assetCategory, const Kiwi::StringPair& parameters );
//
//		/*loads a specific asset group*/
//		void LoadAssetGroup( const std::wstring group );
//
//		/*begins loading of all currently unloaded asset groups*/
//		void Load();
//
//		ASSET_GROUP_STATUS GetGroupLoadStatus( const std::wstring group );
//
//		/*unloads the managed asset with the matching name and category/subcategory*/
//		void UnloadAsset( const std::wstring assetCategory, const std::wstring assetName );
//
//		/*searches the current assets and returns the one with the matching name, or nullptr is a match isnt found*/
//		template<class AssetType>
//		AssetType* FindAsset( const std::wstring assetCategory, const std::wstring assetName )
//		{
//			std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//			return dynamic_cast<AssetType*>(this->_FindAsset( assetCategory, assetName ));
//		}
//
//		/*creates a copy of a model that is currently in the manager as a shared model. the shared model will be
//		identical to the original except that it will also have an instancebuffer that enables it to be efficiently
//		shared between many meshes simultaneously. if an existing shared model with the same name exists, or the
//		source model doesn't exist, a new shared model will not be created and the function will return nullptr.
//		|-ModelType: type of model that is to be copied and created
//		|-sharedModelName: unique name of the new shared model
//		|-sourceModel: name of the existing model that is to be copied*/
//		//template<typename ModelType>
//		//Kiwi::IModel* CreateSharedModel( const std::wstring& sharedModelName, const std::wstring& sourceModel )
//		//{
//		//	std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//		//	//check for any existing shared model with the same name
//		//	Kiwi::IModel* existingShared = dynamic_cast<Kiwi::IModel*>(this->_FindAsset( L"Model", sharedModelName ));
//		//	if( existingShared != nullptr ) return existingShared;
//
//		//	//attempt to find the source model for the copy
//		//	ModelType* existingModel = dynamic_cast<ModelType*>(this->_FindAsset( L"Model", sourceModel ));
//		//	if( existingModel == nullptr ) return nullptr;
//
//		//	//create a new copy of the source model
//		//	std::unique_ptr<ModelType> newModel = std::make_unique<ModelType>( existingModel );
//		//	if( newModel == nullptr ) return nullptr;
//
//		//	Kiwi::IModel* sharedModel = newModel.get();
//		//	sharedModel->SetShared( true );
//		//	sharedModel->SetAssetName( sharedModelName );
//
//		//	//insert the new model into the same group as the original
//		//	auto gItr = m_assetGroups.find( sharedModel->GetAssetCategory() );
//		//	if( gItr != m_assetGroups.end() )
//		//	{
//		//		bool inserted = gItr->second->assets.insert( std::make_pair( sharedModelName, std::move( newModel ) ) ).second;
//		//		if( inserted == false )
//		//		{
//		//			KIWIDEBUGSTRING( Kiwi::ToWString( L"Failed to insert new shared model \"" + sharedModelName + L"\" into asset group \"Model\"\n" ).c_str() );
//		//			return nullptr;
//		//		}
//		//	}
//
//		//	return sharedModel;
//		//}
//
//		template<typename AssetType>
//		AssetType* CopyAsset( const std::wstring& assetCategory, const std::wstring& sourceAssetName, const std::wstring& newAssetName )
//		{
//			std::lock_guard<std::recursive_mutex> lock( m_assetManager_M );
//
//			//get the existing asset
//			Kiwi::IAsset* asset = this->_FindAsset( assetCategory, sourceAssetName );
//			if( asset == nullptr ) return nullptr;
//
//			//clone the existing asset
//			std::unique_ptr<Kiwi::IAsset> newAsset = asset->Clone();
//			if( newAsset == nullptr ) return nullptr;
//
//			newAsset->SetAssetName( newAssetName );
//
//			//insert the new asset into the same group as the original
//			auto gItr = m_assetGroups.find( assetCategory );
//			if( gItr != m_assetGroups.end() )
//			{
//				auto insVal = gItr->second->assets.insert( std::make_pair( newAssetName, std::move( newAsset ) ) );
//				if( insVal.second == false )
//				{
//					KIWIDEBUGSTRING( Kiwi::ToWString( L"Failed to copy asset \"" + sourceAssetName + L"\"" ).c_str() );
//					return nullptr;
//				}
//
//				return dynamic_cast<AssetType*>(insVal.first->second.get());
//
//			}
//			return nullptr;
//		}
//
//	};
//}
//
//#endif