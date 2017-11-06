#ifndef _KIWI_IASSETEVENTLISTENER_H_
#define _KIWI_IASSETEVENTLISTENER_H_

#include "IEventReceiver.h"
#include "AssetEvent.h"

#include <set>
#include <memory>

namespace Kiwi
{

	class IAssetEventBroadcaster;

	class IAssetEventListener:
		private Kiwi::IEventReceiver<AssetEvent>
	{
		friend class IAssetEventBroadcaster;
	private:

		void _ReceiveEvent( const std::shared_ptr<AssetEvent> evt );

	protected:

		virtual void _OnAssetLoaded( const Kiwi::AssetEvent& evt ) {}
		virtual void _OnAssetUnloaded( const Kiwi::AssetEvent& evt ) {}
		virtual void _OnAssetGroupLoaded( const Kiwi::AssetEvent& evt ) {}
		virtual void _OnAssetGroupUnloaded( const Kiwi::AssetEvent& evt ) {}

	public:

		IAssetEventListener() {}
		virtual ~IAssetEventListener() = 0;

	};
};

#endif