#ifndef _KIWI_UIBUTTONEVENTBROADCASTER_H_
#define _KIWI_UIBUTTONEVENTBROADCASTER_H_

#include "EventBroadcaster.h"
#include "UIButtonEventListener.h"

namespace Kiwi
{

	class UIButtonEvent;

	class UIButtonEventBroadcaster :
		private Kiwi::EventBroadcaster<Kiwi::UIButtonEventListener, Kiwi::UIButtonEvent>
	{
	public:

		UIButtonEventBroadcaster() :
			EventBroadcaster<Kiwi::UIButtonEventListener, Kiwi::UIButtonEvent>(&Kiwi::UIButtonEventListener::OnButtonEvent) {}

		virtual ~UIButtonEventBroadcaster() {}

		void AddButtonEventListener( Kiwi::UIButtonEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void RemoveButtonEventListener( Kiwi::UIButtonEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void BroadcastButtonEvent( const Kiwi::UIButtonEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

		void RemoveAllButtonEventListeners()
		{
			this->_RemoveAllListeners();
		}

	};
};

#endif