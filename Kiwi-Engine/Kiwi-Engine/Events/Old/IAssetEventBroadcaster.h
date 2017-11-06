#ifndef _KIWI_IASSETEVENTBROADCASTER_H_
#define _KIWI_IASSETEVENTBROADCASTER_H_

#include "IEventBroadcaster.h"
#include "IAssetEventListener.h"

namespace Kiwi
{

	class IAssetEventBroadcaster :
		private Kiwi::IEventBroadcaster<Kiwi::IAssetEventListener, Kiwi::AssetEvent>
	{
	public:

		IAssetEventBroadcaster() :
			Kiwi::IEventBroadcaster<Kiwi::IAssetEventListener, Kiwi::AssetEvent>( &Kiwi::IAssetEventListener::_ReceiveEvent ) {}

		virtual ~IAssetEventBroadcaster() {}

		void AddAssetListener( Kiwi::IAssetEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void RemoveAssetListener( Kiwi::IAssetEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void RemoveAllAssetListeners()
		{
			this->_RemoveAllListeners();
		}

		void BroadcastAssetEventQueue()
		{
			this->_BroadcastEventQueue();
		}

		void BroadcastAssetEvent( const Kiwi::AssetEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

		void EnqueueAssetEvent( const Kiwi::AssetEvent& evt )
		{
			this->_EnqueueEvent( evt );
		}

	};

}

#endif