#include "IAssetEventListener.h"

#include "IAssetEventBroadcaster.h"

namespace Kiwi
{

	IAssetEventListener::~IAssetEventListener(){}

	void IAssetEventListener::_ReceiveEvent( const std::shared_ptr<AssetEvent> evt )
	{
		if( evt == 0 ) return;

		if( evt != 0 )
		{
			switch( evt->GetEventType() )
			{
				case AssetEvent::ASSET_LOADED:
					{
						_OnAssetLoaded( *evt );
						break;
					}
				case AssetEvent::ASSET_UNLOADED:
					{
						_OnAssetUnloaded( *evt );
						break;
					}
				case AssetEvent::ASSET_GROUP_LOADED:
					{
						this->_OnAssetGroupLoaded( *evt );
						break;
					}
				case AssetEvent::ASSET_GROUP_UNLOADED:
					{
						this->_OnAssetGroupUnloaded( *evt );
						break;
					}
				default:break;
			}
		}
	}

}