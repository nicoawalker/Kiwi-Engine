#ifndef _KIWI_IWINDOWEVENTBROADCASTER_H_
#define _KIWI_IWINDOWEVENTBROADCASTER_H_

#include "EventBroadcaster.h"
#include "IWindowEventListener.h"
#include "WindowEvent.h"

namespace Kiwi
{

	class IWindowEventBroadcaster :
		private Kiwi::EventBroadcaster<Kiwi::IWindowEventListener, Kiwi::WindowEvent>
	{
	public:

		IWindowEventBroadcaster() :
			EventBroadcaster<Kiwi::IWindowEventListener, Kiwi::WindowEvent>(&Kiwi::IWindowEventListener::_OnWindowEvent) {}

		virtual ~IWindowEventBroadcaster() {}

		void AddWindowEventListener( Kiwi::IWindowEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void RemoveWindowEventListener( Kiwi::IWindowEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void BroadcastWindowEvent( const Kiwi::WindowEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

		void RemoveAllWindowEventListeners()
		{
			this->_RemoveAllListeners();
		}

	};
};

#endif