#include "IGlobalEventBroadcaster.h"
#include "IGlobalEventListener.h"

#include <Windows.h>

namespace Kiwi
{

	void IGlobalEventBroadcaster::BroadcastGlobalEvent( GlobalEventPtr e )
	{

		if( e != 0 )
		{
			auto gelItr = m_globalEventListeners.find( e->GetID() );
			if( gelItr != m_globalEventListeners.end() )
			{
				for( auto listenerItr = gelItr->second.begin(); listenerItr != gelItr->second.end(); listenerItr++ )
				{
					if( *listenerItr != 0 )
					{
						(*listenerItr)->EnqueueGlobalEvent( e );
					}
				}
			}
			
			if( e->GetID() != 0 )
			{
				//also need to send the event to the listeners that are listening for all events
				gelItr = m_globalEventListeners.find( 0 );
				if( gelItr != m_globalEventListeners.end() )
				{
					for( auto listenerItr = gelItr->second.begin(); listenerItr != gelItr->second.end(); listenerItr++ )
					{
						if( *listenerItr != 0 )
						{
							(*listenerItr)->EnqueueGlobalEvent( e );
						}
					}
				}
			}
		}

	}

	void IGlobalEventBroadcaster::AddGlobalEventListener( Kiwi::IGlobalEventListener* listener )
	{

		if( listener != 0 )
		{
			if( listener->GetRegisteredEventIDs().size() > 0 )
			{
				//iterate over each event ID the listener wants to receive and add the listener to the list for each
				for( auto eventIDItr = listener->GetRegisteredEventIDs().begin(); eventIDItr != listener->GetRegisteredEventIDs().end(); eventIDItr++ )
				{
					auto gelItr = m_globalEventListeners.find( *eventIDItr );
					if( gelItr != m_globalEventListeners.end() )
					{
						gelItr->second.insert( listener );

					} else
					{
						std::unordered_set<Kiwi::IGlobalEventListener*> newSet;
						newSet.insert( listener );
						m_globalEventListeners.insert( std::make_pair( *eventIDItr, newSet ) );
					}
				}

			} else
			{
				//no event IDs specified, add the listener to the all event list
				auto gelItr = m_globalEventListeners.find( 0 );
				if( gelItr != m_globalEventListeners.end() )
				{
					gelItr->second.insert( listener );

				} else
				{
					std::unordered_set<Kiwi::IGlobalEventListener*> newSet;
					newSet.insert( listener );
					m_globalEventListeners.insert( std::make_pair( 0, newSet ) );
				}
			}
		}

	}

	void IGlobalEventBroadcaster::RemoveGlobalEventListener( Kiwi::IGlobalEventListener* listener )
	{

		if( listener != 0 && m_globalEventListeners.size() > 0 )
		{
			if( listener->GetRegisteredEventIDs().size() > 0 )
			{
				//iterate over each event ID the listener wants to receive and remove the listener from the list for each
				for( auto eventIDItr = listener->GetRegisteredEventIDs().begin(); eventIDItr != listener->GetRegisteredEventIDs().end(); eventIDItr++ )
				{
					auto itr = m_globalEventListeners.find( *eventIDItr );
					if( itr != m_globalEventListeners.end() )
					{
						itr->second.erase( listener );
					}
				}

			} else
			{
				//no event IDs specified, remove the listener from the all event list
				auto itr = m_globalEventListeners.find( 0 );
				if( itr != m_globalEventListeners.end() )
				{
					itr->second.erase( listener );
				}

			}
		}

	}

}