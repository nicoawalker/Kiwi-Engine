#ifndef _KIWI_IASSET_H_
#define _KIWI_IASSET_H_

#include "IThreadSafe.h"
#include "IReferencedObject.h"

#include "..\Types.h"

#include <string>
#include <mutex>
#include <vector>

namespace Kiwi
{

	enum ASSET_LOAD_STATE { UNREADY = 0, LOADING, READY, FAILED };

	class IAsset :
		public IThreadSafe,
		public Kiwi::IReferencedObject
	{
	protected:

		/*stores the current load state.
		all assets are UNLOADED on creation*/
		ASSET_LOAD_STATE m_assetLoadState;

		//stores the main category of asset this is (e.g. Texture, Mesh, etc)
		std::wstring m_assetCategory;

		/*stores the subcategory of the asset (e.g. StaticMesh, DynamicMesh)*/
		std::wstring m_assetSubcategory;

		//the (unique) name of the asset
		std::wstring m_assetName;

		//vector containing the files that this asset uses
		Kiwi::StringPair m_assetProperties;

		//id # unique to this asset
		long m_UID;

	public:

		IAsset( const std::wstring& name, const std::wstring& assetCategory );

		IAsset( const std::wstring& name, const std::wstring& assetCategory, const std::wstring& assetSubcategory );

		/*copies the contents of an existing asset into this one. the existing asset's UID will NOT be
		copied, a new one will be created for this asset*/
		IAsset( const Kiwi::IAsset& source );

		virtual ~IAsset() = 0;

		void AddProperty( const std::wstring& propertyID, const std::wstring& propertyValue ) { m_assetProperties.insert( std::make_pair( propertyID, propertyValue ) ); }
		void AddProperties( const Kiwi::StringPair& properties );

		/*creates a copy of the asset and returns control of the newly created asset to the caller*/
		virtual std::unique_ptr<Kiwi::IAsset> Clone() = 0;

		virtual void SetLoadState( ASSET_LOAD_STATE loadState ) { m_assetLoadState = loadState; }

		void SetAssetName( const std::wstring& name ) { m_assetName = name; }
		void SetAssetCategory( const std::wstring& category ) { m_assetCategory = category; }
		void SetAssetSubcategory( const std::wstring& subcategory ) { m_assetSubcategory = subcategory; }

		virtual ASSET_LOAD_STATE GetLoadState()const { return m_assetLoadState;  }

		long GetUID()const { return m_UID; }

		virtual std::wstring GetAssetSubcategory()const { return m_assetSubcategory;  }
		virtual std::wstring GetAssetName()const { return m_assetName; }
		std::wstring GetAssetCategory()const { return m_assetCategory; }

		/*returns the value associated with the provided property ID. if no such property ID exists,
		an empty string is returned instead*/
		std::wstring GetProperty( const std::wstring& propertyID )const;

		Kiwi::StringPair GetProperties()const { return m_assetProperties; }

	};

}

#endif