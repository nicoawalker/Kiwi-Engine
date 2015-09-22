#ifndef _KIWI_FRAMEEVENTLISTENER_H_
#define _KIWI_FRAMEEVENTLISTENER_H_

#include "FrameEvent.h"

namespace Kiwi
{

	class IFrameEventListener
	{
	public:

		IFrameEventListener() {}
		virtual ~IFrameEventListener() {}

		virtual void OnFrameEvent( const Kiwi::FrameEvent& evt );

		virtual void OnUpdate( const Kiwi::FrameEvent& evt ) {}
		virtual void OnFixedUpdate( const Kiwi::FrameEvent& evt ) {}

	};

}

#endif