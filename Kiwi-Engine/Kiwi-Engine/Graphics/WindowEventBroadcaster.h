#ifndef _KIWI_WINDOWEVENTBROADCASTER_H_
#define _KIWI_WINDOWEVENTBROADCASTER_H_

#include "..\Core\EventBroadcaster.h"

#include "IWindowEventListener.h"

namespace Kiwi
{

	class WindowEvent;

	class WindowEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::IWindowEventListener, Kiwi::WindowEvent>
	{
	public:

		WindowEventBroadcaster() :
			EventBroadcaster<Kiwi::IWindowEventListener, Kiwi::WindowEvent>(&Kiwi::IWindowEventListener::OnWindowEvent) {}

		virtual ~WindowEventBroadcaster() {}

	};
};

#endif