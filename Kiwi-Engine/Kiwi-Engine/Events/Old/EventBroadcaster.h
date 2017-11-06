#ifndef _KIWI_EVENTBROADCASTER_H_
#define _KIWI_EVENTBROADCASTER_H_

#include "Assert.h"

#include "..\Core\Utilities.h"

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

		/*buffers into which new listeners are added before being added to the main listener list
		this allows listeners to insert and remove listeners safely from within the callback function*/
		std::vector<ListenerType*> m_newListenerBuffer;
		std::vector<ListenerType*> m_erasedListenerBuffer;

		//pointer to the callback function that should be called for each listener
		void (ListenerType::*m_eventCallback)(const EventType&);

	protected:

		/*broadcasts the event to each of the registered listeners*/
		void _BroadcastEvent( const EventType& evt )
		{

			/*first process the buffers to add or remove listeners*/
			for( auto eraseItr = m_erasedListenerBuffer.begin(); eraseItr != m_erasedListenerBuffer.end(); )
			{
				/*remove listeners*/
				m_listeners.erase( std::remove( m_listeners.begin(), m_listeners.end(), (*eraseItr) ), m_listeners.end() );
				eraseItr = m_erasedListenerBuffer.erase( eraseItr );
			}

			if( m_newListenerBuffer.size() > 0 )
			{
				/*add the new listeners*/
				m_listeners.insert( m_listeners.end(), m_newListenerBuffer.begin(), m_newListenerBuffer.end() );
				m_newListenerBuffer.clear();
			}

			//send the event to the callback function of each listener
			auto it = m_listeners.begin();
			while( it != m_listeners.end() )
			{
				if( (*it) == 0 )
				{
					it = m_listeners.erase( it );
				} else
				{
					((*it)->*m_eventCallback)(evt);
					it++;
				}
			}
		}

		/*adds the listener to the list of listeners registered to this broadcaster*/
		void _AddListener( ListenerType* listener )
		{

			if( listener == 0 ) return;

			/*first check if the given listener is already registered*/
			for( auto it = m_listeners.begin(); it != m_listeners.end(); it++ )
			{
				if( (*it) == listener )
				{
					return;
				}
			}

			m_newListenerBuffer.push_back( listener );
		}

		/*removes the listener to the list of listeners registered to this broadcaster*/
		void _RemoveListener( ListenerType* listener )
		{
			if( listener )
			{
				m_erasedListenerBuffer.push_back( listener );
			}
		}

		void _RemoveAllListeners()
		{
			Kiwi::FreeMemory( m_listeners );
		}

	public:

		EventBroadcaster(void (ListenerType :: *eventCallback)(const EventType&)) 
		{ 
			m_eventCallback = eventCallback; 
		}

		virtual ~EventBroadcaster()
		{
			Kiwi::FreeMemory(m_listeners);
		}		

	};
};

#endif