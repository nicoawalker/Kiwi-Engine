#ifndef _KIWI_UISCROLLBAREVENTLISTENER_H_
#define _KIWI_UISCROLLBAREVENTLISTENER_H_

#include "UIScrollBarEvent.h"

namespace Kiwi
{

	class UIScrollBarEventBroadcaster;

	class UIScrollBarEventListener
	{
	friend class UIScrollBarEventBroadcaster;
	public:

		UIScrollBarEventListener(){}
		virtual ~UIScrollBarEventListener(){}

		virtual void OnScrollEvent( const Kiwi::UIScrollBarEvent& evt ) {}

	};
};

#endif