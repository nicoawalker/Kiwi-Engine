#include "IAsset.h"
#include "Utilities.h"

namespace Kiwi
{

	IAsset::IAsset( const std::wstring& name, const std::wstring& assetCategory ):
		Kiwi::IReferencedObject( name )
	{
		m_assetLoadState = ASSET_LOAD_STATE::UNREADY;
		m_assetName = name;
		m_assetCategory = assetCategory;
		m_assetSubcategory = m_assetCategory;

		m_UID = Kiwi::NewUID();
	}

	IAsset::IAsset( const std::wstring& name, const std::wstring& assetCategory, const std::wstring& assetSubcategory ) :
		Kiwi::IReferencedObject( name )
	{
		m_assetLoadState = ASSET_LOAD_STATE::UNREADY;
		m_assetName = name;
		m_assetCategory = assetCategory;
		m_assetSubcategory = assetSubcategory;

		m_UID = Kiwi::NewUID();
	}

	IAsset::IAsset( const Kiwi::IAsset& source ) :
		Kiwi::IReferencedObject( source.GetAssetName() )
	{
		m_assetLoadState = source.m_assetLoadState;
		m_assetCategory = source.m_assetCategory;
		m_assetSubcategory = source.m_assetSubcategory;
		m_assetName = source.m_assetName;
		m_assetProperties = source.m_assetProperties;

		m_UID = Kiwi::NewUID(); //UIDs cannot be copied or they wouldn't be UIDs

		m_referenceCount = source.m_referenceCount;
	}

	IAsset::~IAsset()
	{

		Kiwi::FreeMemory( m_assetProperties );

	}

	void IAsset::AddProperties( const Kiwi::StringPair& properties )
	{
		for( auto itr = properties.begin(); itr != properties.end(); itr++ )
		{
			this->AddProperty( itr->first, itr->second );
		}
	}

	std::wstring IAsset::GetProperty( const std::wstring& propertyID )const
	{
		auto itr = m_assetProperties.find( propertyID );
		if( itr != m_assetProperties.end() )
		{
			return itr->second;
		}

		return L"";
	}

}