#ifndef _KIWI_IFRAMEEVENTBROADCASTER_H_
#define _KIWI_IFRAMEEVENTBROADCASTER_H_

#include "EventBroadcaster.h"
#include "IFrameEventListener.h"

namespace Kiwi
{

	class FrameEvent;

	class IFrameEventBroadcaster :
		private Kiwi::EventBroadcaster<Kiwi::IFrameEventListener, Kiwi::FrameEvent>
	{
	public:

		IFrameEventBroadcaster() :
			EventBroadcaster<Kiwi::IFrameEventListener, Kiwi::FrameEvent>( &Kiwi::IFrameEventListener::_OnFrameEvent ) {}

		virtual ~IFrameEventBroadcaster() {}

		void AddFrameEventListener( Kiwi::IFrameEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void BroadcastFrameEvent( const Kiwi::FrameEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

		void RemoveFrameEventListener( Kiwi::IFrameEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void RemoveAllFrameEventListeners()
		{
			this->_RemoveAllListeners();
		}

	};
}

#endif