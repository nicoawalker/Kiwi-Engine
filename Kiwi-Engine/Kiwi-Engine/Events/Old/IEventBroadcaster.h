#ifndef _KIWI_IEVENTBROADCASTER_H_
#define _KIWI_IEVENTBROADCASTER_H_

#include "..\Core\Utilities.h"

#include <set>
#include <deque>
#include <mutex>
#include <memory>

namespace Kiwi
{

	template<typename ListenerType, typename EventType>
	class IEventBroadcaster
	{
	private:

		//stores all of the registered listeners
		std::set<std::shared_ptr<ListenerType>> m_listeners;

		std::deque<std::shared_ptr<EventType>> m_eventQueue;

		/*buffer/copy of the listeners set used so that listeners can remove themselves during an event call*/
		std::set<std::shared_ptr<ListenerType>> m_newListeners;

		//pointer to the callback function that should be called for each listener
		void (ListenerType::*m_eventCallback)(const std::shared_ptr<EventType>);

		std::recursive_mutex m_broadcasterMutex;

	private:

		void _AddNewListeners()
		{
			if( m_newListeners.size() > 0 )
			{
				for( auto& sharePtr : m_newListeners )
				{
					m_listeners.insert( sharePtr );
				}
			}
			Kiwi::FreeMemory( m_newListeners );
			m_listeners.insert( m_listenerBuffer.begin(), m_listenerBuffer.end() );
			m_swapBuffer = false;
		}

	protected:

		/*broadcasts all events in the event queue*/
		void _BroadcastEventQueue()
		{

			std::lock_guard<std::recursive_mutex> lock( m_broadcasterMutex );

			if( m_swapBuffer ) this->_ProcessListenerBuffers();

			//send each event to the callback function of each listener
			while( m_eventQueue.size() > 0 )
			{
				for( auto it = m_listeners.begin(); it != m_listeners.end(); )
				{
					((*it)->*m_eventCallback)(m_eventQueue.front());
					it++;
				}
				m_eventQueue.pop_front();
			}

		}

		/*does not add the event to the event queue, instead immediately sending the event to all listeners*/
		void _BroadcastEvent( const EventType& evt )
		{

			std::lock_guard<std::recursive_mutex> lock( m_broadcasterMutex );

			if( m_swapBuffer ) this->_ProcessListenerBuffers();

			//send the event to the callback function of each listener
			for( auto it = m_listeners.begin(); it != m_listeners.end(); )
			{
				if( (*it) == 0 )
				{
					it = m_listeners.erase( it );

				} else
				{
					std::shared_ptr<EventType> sharedPtr = std::make_shared<EventType>( evt );
					((*it)->*m_eventCallback)( sharedPtr );
					it++;
				}
			}

		}

		/*adds the event into the event queue to be broadcast when _BroadcastEventQueue() is called*/
		void _EnqueueEvent( const EventType& evt )
		{

			std::lock_guard<std::recursive_mutex> lock( m_broadcasterMutex );

			m_eventQueue.push_back( std::make_shared<EventType>( evt ) );

		}

		/*adds the listener to the list of listeners registered to this broadcaster*/
		void _AddListener( ListenerType* listener )
		{

			std::lock_guard<std::recursive_mutex> lock( m_broadcasterMutex );

			if( listener == 0 ) return;

			/*first check if the given listener is already registered*/
			auto itr = m_listenerBuffer.find( listener );
			if( itr == m_listenerBuffer.end() )
			{
				m_listenerBuffer.insert( listener );
				m_swapBuffer = true;
			}
		}

		/*removes the listener to the list of listeners registered to this broadcaster*/
		void _RemoveListener( ListenerType* listener )
		{

			if( listener == 0 ) return;

			std::lock_guard<std::recursive_mutex> lock( m_broadcasterMutex );

			auto itr = m_listenerBuffer.find( listener );
			if( itr == m_listenerBuffer.end() )
			{
				m_listenerBuffer.insert( listener );
				m_swapBuffer = true;
			}
		}

		void _RemoveAllListeners()
		{

			std::lock_guard<std::recursive_mutex> lock( m_broadcasterMutex );

			Kiwi::FreeMemory( m_listeners );
			Kiwi::FreeMemory( m_listenerBuffer );
		}

	public:

		IEventBroadcaster( void (ListenerType::*eventCallback)(const std::shared_ptr<EventType>) ) :
			m_eventCallback( eventCallback ), m_swapBuffer( false) {}

	};

}


#endif