#ifndef _KIWI_ASSETEVENTDISPATCHER_H_
#define _KIWI_ASSETEVENTDISPATCHER_H_

#include "IEventDispatcher.h"
#include "AssetEvent.h"

namespace Kiwi
{

	class AssetEventDispatcher :
		public Kiwi::IEventDispatcher<Kiwi::AssetEvent>
	{
	public:

		~AssetEventDispatcher(){}

		void RegisterAssetEventReceiver( IEventReceiver<Kiwi::AssetEvent>& receiver )
		{
			this->_RegisterReceiver( receiver );
		}

		void UnregisterAssetEventReceiver( IEventReceiver<Kiwi::AssetEvent>& receiver )
		{
			this->_UnregisterReceiver( receiver );
		}

		void DispatchAssetEvent( std::shared_ptr<Kiwi::AssetEvent> evt )
		{
			this->_DispatchEvent( evt );
		}

	};
}

#endif