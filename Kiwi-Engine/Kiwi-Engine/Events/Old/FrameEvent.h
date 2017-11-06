#ifndef _KIWI_FRAMEEVENT_H_
#define _KIWI_FRAMEEVENT_H_

#include "Event.h"

namespace Kiwi
{

	class EngineRoot;

	class FrameEvent:
		public Kiwi::Event<Kiwi::EngineRoot>
	{
	public:

		enum EventType { UPDATE, FIXED_UPDATE, RENDER_START, RENDER_END };

	protected:

		EventType m_eventType;

	public:

		FrameEvent( Kiwi::EngineRoot& engine, EventType eventType ) :
			Event( &engine ),
			m_eventType( eventType ) {}

		virtual ~FrameEvent() {}

		Kiwi::FrameEvent::EventType GetEventType()const { return m_eventType; }

	};
}

#endif