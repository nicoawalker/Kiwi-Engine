#ifndef _KIWI_IWINDOWEVENTLISTENER_H_
#define _KIWI_IWINDOWEVENTLISTENER_H_

#include "WindowEvent.h"

namespace Kiwi
{

	class IWindowEventListener
	{
	public:

		IWindowEventListener(){}
		virtual ~IWindowEventListener(){}

		virtual void OnWindowEvent( const Kiwi::WindowEvent& evt ) = 0;

	};
};

#endif