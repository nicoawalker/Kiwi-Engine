#ifndef _KIWI_EVENTBROADCASTER_H_
#define _KIWI_EVENTBROADCASTER_H_

#include "Utilities.h"
#include "Assert.h"

#include <vector>
#include <algorithm>
#include <Windows.h>

namespace Kiwi
{

	template<class ListenerType, class EventType>
	class EventBroadcaster
	{
	private:

		//stores all of the registered listeners
		std::vector<ListenerType*> m_listeners;

		//pointer to the callback function that should be called for each listener
		void (ListenerType::*m_eventCallback)(const EventType&);

	public:

		EventBroadcaster(void (ListenerType :: *eventCallback)(const EventType&)) 
		{ 
			m_eventCallback = eventCallback; 
		}

		virtual ~EventBroadcaster()
		{

			auto itr = m_listeners.begin();
			for( ; itr != m_listeners.end(); itr++ )
			{
				SAFE_DELETE( *itr );
			}

			Kiwi::FreeMemory(m_listeners);
		}

		virtual void AddListener(ListenerType* listener) 
		{ 

			if(listener) m_listeners.push_back(listener); 
		}

		virtual void RemoveListener(ListenerType* listener) 
		{ 

			if(listener)
			{
				m_listeners.erase( std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
			}
		}

		virtual void RemoveAllListeners()
		{

			auto itr = m_listeners.begin();
			for( ; itr != m_listeners.end(); itr++ )
			{
				SAFE_DELETE( *itr );
			}

			Kiwi::FreeMemory( m_listeners );
		}

		virtual void BroadcastEvent(const EventType& evt)
		{

			//send the event to the callback function of each listener
			auto it = m_listeners.begin();
			while( it != m_listeners.end() )
			{
				if( (*it) == 0)
				{
					it = m_listeners.erase(it);
				}else
				{
					( (*it)->*m_eventCallback)(evt);
					it++;
				}
			}
		}

	};
};

#endif