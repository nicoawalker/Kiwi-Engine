#ifndef _KIWI_IWINDOWEVENTLISTENER_H_
#define _KIWI_IWINDOWEVENTLISTENER_H_

#include "WindowEvent.h"

namespace Kiwi
{

	class IWindowEventBroadcaster;

	class IWindowEventListener
	{
		friend class IWindowEventBroadcaster;
	protected:

		virtual void _OnWindowEvent( const Kiwi::WindowEvent& evt ) {};

	public:

		IWindowEventListener(){}
		virtual ~IWindowEventListener(){}

	};
};

#endif