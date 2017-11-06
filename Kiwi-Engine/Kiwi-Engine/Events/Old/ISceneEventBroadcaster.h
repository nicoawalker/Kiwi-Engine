#ifndef _KIWI_ISCENEEVENTBROADCASTER_H_
#define _KIWI_ISCENEEVENTBROADCASTER_H_

#include "EventBroadcaster.h"
#include "ISceneEventListener.h"

namespace Kiwi
{

	class ISceneEventBroadcaster :
		private Kiwi::EventBroadcaster<Kiwi::ISceneEventListener, Kiwi::SceneEvent>
	{
	public:

		ISceneEventBroadcaster() :
			Kiwi::EventBroadcaster<Kiwi::ISceneEventListener, Kiwi::SceneEvent>( &Kiwi::ISceneEventListener::_OnSceneEvent ) {}

		virtual ~ISceneEventBroadcaster() {}

		void AddSceneListener( Kiwi::ISceneEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void RemoveSceneListener( Kiwi::ISceneEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void BroadcastSceneEvent( const Kiwi::SceneEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

		void RemoveAllSceneListeners()
		{
			this->_RemoveAllListeners();
		}

	};
};

#endif