#include "IAssetConsumer.h"
#include "EngineRoot.h"

namespace Kiwi
{

	Kiwi::EventResponse IAssetConsumer::_AssetCallback( std::shared_ptr<AssetEvent> evt )
	{
		if( evt == nullptr || evt->source == nullptr ) return Kiwi::EventResponse::NONE;

		std::wstring assetName = evt->source->GetAssetName();

		auto itr = m_awaitedAssets.find( assetName );
		if( itr != m_awaitedAssets.end() )
		{
			if( this->_OnAssetFetched( evt->source ) == Kiwi::EventResponse::DISCONNECT_THIS )
			{
				m_awaitedAssets.erase( itr );
			}
		}
		
		return (m_awaitedAssets.size() == 0) ? Kiwi::EventResponse::DISCONNECT_THIS : Kiwi::EventResponse::NONE;
	}

	void IAssetConsumer::FetchAsset( const std::wstring& assetCategory, const std::wstring& assetName, bool addListenerOnFail )
	{
		Kiwi::IAsset* asset = _kAssetManager.FindAsset<Kiwi::IAsset>( assetCategory, assetName );

		if( asset == nullptr && addListenerOnFail )
		{
			/*add this class as a listener for asset loaded events so it will be notified when the
			desired asset has been loaded*/
			_kAssetManager.ConnectAssetEventReceiver( *this, L"AssetLoaded", &IAssetConsumer::_AssetCallback );
			m_awaitedAssets.insert( assetName );

			return;
		}

		this->_OnAssetFetched( asset );
	}

}