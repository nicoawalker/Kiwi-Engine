#ifndef _KIWI_IASSETCONSUMER_H_
#define _KIWI_IASSETCONSUMER_H_

#include "IAsset.h"

#include "../Events/EventHandling.h"

#include <unordered_set>

namespace Kiwi
{

	class IAssetConsumer:
		public Kiwi::IAssetEventReceiver
	{
	protected:

		std::unordered_set<std::wstring> m_awaitedAssets;

	private:

		Kiwi::EventResponse _AssetCallback( std::shared_ptr<AssetEvent> evt );

	protected:

		virtual Kiwi::EventResponse _OnAssetFetched( Kiwi::IAsset* asset ) = 0;

	public:

		IAssetConsumer(){}
		~IAssetConsumer(){}

		/*attempts to fetch an asset. if the asset is found it is passed to the callers _OnAssetFetched method. 
		if the asset is not found  and addListenerOnFail is true then the consumer will listen for the asset to 
		be loaded and when it is it will be passed to the callers _OnAssetFetched method
		|-assetCategory: category of asset to be fetched
		|-assetName: name of asset to be fetched
		|-addListenerOnFail: if true the consumer will listen for asset events if the desired asset
		  is not immediately found*/
		void FetchAsset( const std::wstring& assetCategory, const std::wstring& assetName, bool addListenerOnFail = true );

	};
}

#endif