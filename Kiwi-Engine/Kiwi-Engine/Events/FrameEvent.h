#ifndef _KIWI_FRAMEEVENT_H_
#define _KIWI_FRAMEEVENT_H_

#include "IEvent.h"

namespace Kiwi
{

	class EngineRoot;

	enum FrameEventType { UPDATE, FIXED_UPDATE };

	class FrameEvent :
		public Kiwi::IEvent<Kiwi::EngineRoot>
	{
	public:

		FrameEventType eventType;

	public:

		FrameEvent( Kiwi::EngineRoot& source, FrameEventType evtType ) :
			IEvent<Kiwi::EngineRoot>( &source, L"" ),
			eventType(evtType) {}

		virtual ~FrameEvent() {}

	};

};

#endif