#ifndef _KIWI_IFRAMEEVENTLISTENER_H_
#define _KIWI_IFRAMEEVENTLISTENER_H_

#include "FrameEvent.h"

namespace Kiwi
{

	class IFrameEventBroadcaster;

	class IFrameEventListener
	{
		friend class IFrameEventBroadcaster;
	private:

		void _OnFrameEvent( const Kiwi::FrameEvent& evt );

	protected:

		virtual void _OnUpdate() {}
		virtual void _OnFixedUpdate() {}
		virtual void _OnRenderStart() {}
		virtual void _OnRenderEnd() {}

	public:

		IFrameEventListener() {}
		virtual ~IFrameEventListener() {}

	};

}

#endif