#ifndef _KIWI_IASSET_H_
#define _KIWI_IASSET_H_

#include "IThreadSafe.h"

#include <string>
#include <mutex>
#include <vector>

namespace Kiwi
{

	enum ASSET_LOAD_STATE { UNLOADED = 0, LOADING, LOADED, LOAD_FAILED };

	class IAsset : 
		public IThreadSafe
	{
	protected:

		/*stores the current load state.
		all assets are UNLOADED on creation*/
		ASSET_LOAD_STATE m_assetLoadState;

		//stores the type of asset this is (e.g. Texture, Mesh, etc)
		std::wstring m_assetType;
		//the (unique) name of the asset
		std::wstring m_assetName;
		//vector containing the files that this asset uses
		std::vector<std::wstring> m_assetFiles;

		//id # unique to this asset
		unsigned long m_assetID;

		//current ID number that will be assigned to the next asset that is created
		static unsigned long m_globalAssetID;

	public:

		IAsset( std::wstring name, std::wstring assetType );
		virtual ~IAsset() = 0;

		virtual void SetLoadState( ASSET_LOAD_STATE loadState ) { m_assetLoadState = loadState; }

		virtual ASSET_LOAD_STATE GetLoadState()const { return m_assetLoadState;  }

		virtual unsigned long GetAssetID()const { return m_assetID; }

		virtual std::wstring GetAssetType()const { return m_assetType;  }
		virtual std::wstring GetAssetName()const { return m_assetName; }
		virtual std::wstring GetAssetFile( unsigned int fileIndex )const;

	};

}

#endif