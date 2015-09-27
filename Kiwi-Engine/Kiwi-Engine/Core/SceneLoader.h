#ifndef _KIWI_SCENELOADER_H_
#define _KIWI_SCENELOADER_H_

#include "Exception.h"
#include "IAsset.h"

#include <string>
#include <deque>
#include <vector>
#include <mutex>
#include <thread>

namespace Kiwi
{

	class Scene;
	//class IAsset;
	class D3DAssetLoader;

	//stores information used to create an asset once loading starts
	struct AssetDesc
	{
		//name of the files used by the asset
		std::vector<std::wstring> files;
		//name of the asset once loaded
		std::wstring name;
		//what kind of asset this is
		std::wstring assetType;
	};	

	class SceneLoader
	{
	protected:

		//contains the unique id # that will be assigned to the next created asset
		struct AssetID
		{
			static unsigned long id;
			std::mutex guard;
		};
		AssetID m_assetID;

		Kiwi::Scene* m_scene;

		std::deque<AssetDesc> m_loadQueue;

		//stores all of the exceptions caught from the loading threads
		std::vector<Kiwi::Exception> m_caughtExceptions;

		std::mutex m_loadQueueMutex;
		std::mutex m_exceptionMutex;
		std::mutex m_byteMutex; //mutex for the m_loadedSizeInBytes variable

		std::thread m_loadQueueThread;

		//size of all of the assets that are to be loaded, obtained using sizeof() on their types
		unsigned long m_totalSizeInBytes;
		//size of all of the assets that have been loaded
		unsigned long m_loadedSizeInBytes;

		unsigned int m_totalQueuedAssets;
		unsigned int m_numAssetsLoaded;

		//stores whether or not the loader is in the process of loading
		bool m_isLoading;

	private:

		//checks whether a type is a built in type or a custom type
		bool _CheckType( std::wstring assetType );

	protected:

		/*thread that loads a single built in asset type. 
		these threads are created within the _LoadQueue thread*/
		Kiwi::IAsset* _LoadAsset( Kiwi::AssetDesc assetDesc );

		/*can be overridden to load custom  non-engine-type assets*/
		Kiwi::IAsset* _LoadCustomAsset( Kiwi::AssetDesc assetDesc ) { return 0; }

		/*this thread loads all of the assets currently in the load queue*/
		void _LoadQueue( unsigned long maxThreads );

		Kiwi::IAsset* _LoadTexture( std::wstring name, std::wstring textureFile );
		Kiwi::IAsset* _LoadOBJMesh( std::wstring name, std::wstring objFile );
		//void _LoadMD5Mesh();
		Kiwi::IAsset* _LoadFont( std::wstring fontFile, std::wstring textureFile );
		Kiwi::IAsset* _LoadShader( std::wstring name, std::wstring vsFile, std::wstring psFile );

	public:

		SceneLoader( Kiwi::Scene* scene );
		~SceneLoader();

		/*adds an asset to the load queue*/
		template<class AssetType>
		void QueueLoading( std::wstring assetType, std::wstring name, std::wstring filepath );

		template<class AssetType>
		void QueueLoading( std::wstring assetType, std::wstring name, std::vector<std::wstring> files );

		void LoadFromFile( std::wstring filename );

		void Start( unsigned long maxThreads = 40 );

		//gets the total number of bytes that have been queued for loading
		unsigned long GetBytesTotal()const { return m_totalSizeInBytes; }
		//gets the number of bytes that have been loaded so far
		unsigned long GetBytesLoaded()const { return m_loadedSizeInBytes; }
		/*returns a vector containing all of the exceptions that were caught during loading*/
		std::vector<Kiwi::Exception> GetExceptions()const { return m_caughtExceptions; }

		bool Finished();

	};	

	template<class AssetType>
	void SceneLoader::QueueLoading( std::wstring assetType, std::wstring name, std::wstring filepath )
	{

		AssetDesc assetDesc;
		assetDesc.name = name;
		assetDesc.assetType = assetType;
		assetDesc.files.push_back(filepath);

		m_totalSizeInBytes += sizeof( AssetType );

		m_loadQueueMutex.lock();
			m_loadQueue.push_back( assetDesc );
		m_loadQueueMutex.unlock();

	}

	template<class AssetType>
	void SceneLoader::QueueLoading( std::wstring assetType, std::wstring name, std::vector<std::wstring> files )
	{

		AssetDesc assetDesc;
		assetDesc.name = name;
		assetDesc.assetType = assetType;
		assetDesc.files.swap(files);

		m_totalSizeInBytes += sizeof( AssetType );

		m_loadQueueMutex.lock();
			m_loadQueue.push_back( assetDesc );
		m_loadQueueMutex.unlock();

	}

}

#endif