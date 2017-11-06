#include "IMeshAsset.h"

namespace Kiwi
{

	IMeshAsset::IMeshAsset( const std::wstring& assetName, const std::wstring& assetCategory, const std::wstring& assetSubcategory ) :
		Kiwi::IAsset( assetName, assetCategory, assetSubcategory )
	{
		m_isShared = false;
	}

	IMeshAsset::IMeshAsset(const Kiwi::IMeshAsset& source):
		Kiwi::IAsset( source )
	{
		m_isShared = source.m_isShared;
	}

	IMeshAsset::~IMeshAsset() {}

}