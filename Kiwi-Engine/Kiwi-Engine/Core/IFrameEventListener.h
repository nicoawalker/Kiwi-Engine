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

		virtual void OnUpdate( Kiwi::EngineRoot* engine ) {}
		virtual void OnFixedUpdate( Kiwi::EngineRoot* engine ) {}

	};

}

#endif