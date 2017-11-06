#ifndef _KIWI_IGLOBALEVENTBROADCASTER_H_
#define _KIWI_IGLOBALEVENTBROADCASTER_H_

#include "IGlobalEvent.h"

#include <unordered_map>
#include <unordered_set>

namespace Kiwi
{

	class IGlobalEventListener;

	class IGlobalEventBroadcaster
	{
	protected:

		/*stores the list of listeners, sorted by the ID of event they are registered for
		and ID of 0 indicates a listener wants to receive ALL global events*/
		std::unordered_map<int, std::unordered_set<Kiwi::IGlobalEventListener*>> m_globalEventListeners;

	public:

		IGlobalEventBroadcaster() {}

		void BroadcastGlobalEvent( GlobalEventPtr e );

		void AddGlobalEventListener( Kiwi::IGlobalEventListener* listener );

		void RemoveGlobalEventListener( Kiwi::IGlobalEventListener* listener );

	};
}

#endif