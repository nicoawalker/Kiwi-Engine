#ifndef _KIWI_IGLOBALEVENTLISTENER_H_
#define _KIWI_IGLOBALEVENTLISTENER_H_

#include "IGlobalEvent.h"

#include <deque>
#include <vector>
#include <memory>

namespace Kiwi
{

	class IGlobalEventBroadcaster;

	class IGlobalEventListener
	{
	protected:

		std::vector<IGlobalEventBroadcaster*> m_broadcasters;

		std::deque<GlobalEventPtr> m_globalEventQueue;

		/*stores a list of event IDs that this listener wants to receive
		if this list is empty, the listener will receive all global events*/
		std::vector<int> m_registeredEventIDs;

	protected:

		virtual void _ProcessGlobalEvent( GlobalEventPtr e ) {}

		//void _AddBroadcaster( IGlobalEventBroadcaster* broadcaster );

	public:

		IGlobalEventListener();
		virtual ~IGlobalEventListener();

		void EnqueueGlobalEvent( GlobalEventPtr& e );

		/*pulls the next event in the event queue and sends it to _ProcessGlobalEvent(IGlobalEvent* e)
		returns true if there are more events in the queue, and false if the queue is empty*/
		bool PullNextEvent();

		/*sets the list of event IDs that this listener wants to receive
		if this list is not set, the listener will receive all global events*/
		void SetRegisteredEventIDs( std::vector<int> eventIDs ) { m_registeredEventIDs = eventIDs; }

		std::vector<int>& GetRegisteredEventIDs() { return m_registeredEventIDs; }

	};
}

#endif