#include "IAsset.h"

namespace Kiwi
{

	unsigned long IAsset::m_globalAssetID = 0;

	IAsset::IAsset( std::wstring name, std::wstring assetType )
	{

		m_mutex->lock();

			m_assetLoadState = UNLOADED;
			m_assetName = name;
			m_assetType = assetType;
			m_assetID = m_globalAssetID++;

		m_mutex->unlock();

	}

	IAsset::~IAsset()
	{
	}

	std::wstring IAsset::GetAssetFile( unsigned int fileIndex )const
	{

		if( fileIndex >= m_assetFiles.size() ) return L"";

		return m_assetFiles[fileIndex];

	}

}