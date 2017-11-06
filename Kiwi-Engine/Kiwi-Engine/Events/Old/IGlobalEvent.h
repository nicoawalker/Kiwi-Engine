#ifndef _KIWI_IGLOBALEVENT_H_
#define _KIWI_IGLOBALEVENT_H_

#include <memory>

namespace Kiwi
{

	class IGlobalEvent
	{
	protected:

		int m_eventID;

	public:

		IGlobalEvent( int eventID ) :
			m_eventID( eventID )
		{
		}

		virtual ~IGlobalEvent() = 0 {}

		int GetID()const { return m_eventID; }

	};

	typedef std::shared_ptr<IGlobalEvent> GlobalEventPtr;

}

#endif