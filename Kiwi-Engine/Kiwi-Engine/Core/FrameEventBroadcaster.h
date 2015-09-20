#ifndef _KIWI_FRAMEEVENTBROADCASTER_H_
#define _KIWI_FRAMEEVENTBROADCASTER_H_

#include "EventBroadcaster.h"
#include "IFrameEventListener.h"

namespace Kiwi
{

	class FrameEvent;

	class FrameEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::IFrameEventListener, Kiwi::FrameEvent>
	{
	public:

		FrameEventBroadcaster() :
			EventBroadcaster<Kiwi::IFrameEventListener, Kiwi::FrameEvent>( &Kiwi::IFrameEventListener::OnFrameEvent ) {}

		virtual ~FrameEventBroadcaster() {}

	};
}

#endif