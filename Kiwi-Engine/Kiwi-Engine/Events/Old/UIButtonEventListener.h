#ifndef _KIWI_UIBUTTONEVENTLISTENER_H_
#define _KIWI_UIBUTTONEVENTLISTENER_H_

#include "UIButtonEvent.h"

namespace Kiwi
{

	class UIButtonEventBroadcaster;

	class UIButtonEventListener
	{
	friend class UIButtonEventBroadcaster;
	private:

		virtual void OnButtonEvent( const Kiwi::UIButtonEvent& evt );

		virtual void _OnButtonPress( const Kiwi::UIButtonEvent& evt ) {}
		virtual void _OnButtonRelease( const Kiwi::UIButtonEvent& evt ) {}
		virtual void _OnButtonHeld( const Kiwi::UIButtonEvent& evt ) {}

	public:

		UIButtonEventListener(){}
		virtual ~UIButtonEventListener(){}

	};
};

#endif