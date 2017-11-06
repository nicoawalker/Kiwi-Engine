#include "IGlobalEventListener.h"

#include "IGlobalEventBroadcaster.h"

#include <Windows.h>

namespace Kiwi
{

	IGlobalEventListener::IGlobalEventListener()
	{

	}

	IGlobalEventListener::~IGlobalEventListener()
	{
		
		//unregister from all broadcasters
		/*for( auto itr = m_broadcasters.begin(); itr != m_broadcasters.end(); itr++ )
		{
			
		}*/

	}

	void IGlobalEventListener::EnqueueGlobalEvent( GlobalEventPtr& e )
	{

		if( e != 0 )
		{
			m_globalEventQueue.push_back( e );
		}

	}

	/*pulls the next event in the event queue and sends it to _ProcessGlobalEvent(IGlobalEvent* e)
	returns true if there are more events in the queue, and false if the queue is empty*/
	bool IGlobalEventListener::PullNextEvent()
	{
		
		if( m_globalEventQueue.size() == 0 )
		{
			return false;
		}
		
		this->_ProcessGlobalEvent( m_globalEventQueue.front() );
		
		m_globalEventQueue.pop_front();
		
		return (m_globalEventQueue.size() == 0) ? false : true;

	}
}