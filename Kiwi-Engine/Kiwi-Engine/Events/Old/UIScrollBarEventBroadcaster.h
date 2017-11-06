#ifndef _KIWI_UISCROLLBAREVENTBROADCASTER_H_
#define _KIWI_UISCROLLBAREVENTBROADCASTER_H_

#include "EventBroadcaster.h"
#include "UIScrollBarEventListener.h"

namespace Kiwi
{

	class UIScrollBarEvent;

	class UIScrollBarEventBroadcaster :
		private Kiwi::EventBroadcaster<Kiwi::UIScrollBarEventListener, Kiwi::UIScrollBarEvent>
	{
	public:

		UIScrollBarEventBroadcaster() :
			EventBroadcaster<Kiwi::UIScrollBarEventListener, Kiwi::UIScrollBarEvent>( &Kiwi::UIScrollBarEventListener::OnScrollEvent ) {}

		virtual ~UIScrollBarEventBroadcaster() {}


		void AddScrollBarEventListener( Kiwi::UIScrollBarEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void RemoveScrollBarEventListener( Kiwi::UIScrollBarEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void BroadcastScrollBarEvent( const Kiwi::UIScrollBarEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

		void RemoveAllScrollBarEventListeners()
		{
			this->_RemoveAllListeners();
		}

	};
};

#endif