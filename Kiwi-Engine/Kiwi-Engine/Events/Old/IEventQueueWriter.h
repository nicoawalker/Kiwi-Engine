#ifndef KIWI_IEVENTQUEUEWRITER_H_
#define KIWI_IEVENTQUEUEWRITER_H_

#include "..\Core\Utilities.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace Kiwi
{

	template<class ReaderType, class EventType>
	class IEventQueueWriter
	{
	private:

		//pointer to the queue message function that should be called for each reader
		void (ReaderType::*m_readerCallback)(const EventType&);

	protected:

		/*stores the list of readers, sorted by the ID of event they are registered for
		and ID of 0 indicates a reader wants to receive ALL global events*/
		std::unordered_map<int, std::unordered_set<ReaderType*>> m_eventReaders;

		void _WriteEvent( const EventType& evt )
		{

			//send the event to the callback function of each listener
			for( auto it = m_eventReaders.begin(); it != m_eventReaders.end(); it++ )
			{
				for( auto readerItr = it->second.begin(); readerItr != it->second.end(); readerItr++ )
				{
					if( (*readerItr) != 0 )
					{
						((*readerItr)->*m_readerCallback)(evt);
					}
				}
			}

		}

		void _AddReader( ReaderType& reader, const std::vector<int>& eventIDs )
		{

			for( auto idItr = eventIDs.begin(); idItr != eventIDs.end(); idItr++ )
			{
				auto readerSet = m_eventReaders.find( *idItr );
				if( readerSet != m_eventReaders.end() )
				{
					auto existingItr = readerSet->second.find( &reader );
					if( existingItr == readerSet->second.end() )
					{
						/*doesnt exist yet, can add it now*/
						readerSet->second.insert( &reader );
					}

				} else
				{
					/*no readers registered for this id yet*/
					std::unordered_set<ReaderType*> tempSet;
					tempSet.insert( &reader );
					m_eventReaders.insert( std::make_pair( *idItr, tempSet ) );
				}
			}

		}

		/*removes the reader from recieving the specified events*/
		void _RemoveReader( ReaderType& reader, const std::vector<int>& eventIDs )
		{

			for( auto idItr = eventIDs.begin(); idItr != eventIDs.end(); idItr++ )
			{
				auto readerSet = m_eventReaders.find( *idItr );
				if( readerSet != m_eventReaders.end() )
				{
					auto existingItr = readerSet->second.find( &reader );
					if( existingItr != readerSet->second.end() )
					{
						existingItr = readerSet->second.erase( existingItr );
					}
				}
			}

		}

		/*removes the reader from recieving all events*/
		void _RemoveReader( ReaderType& reader )
		{

			for( auto readerItr = m_eventReaders.begin(); readerItr != m_eventReaders.end(); readerItr++ )
			{
				auto existingItr = readerItr->second.find( &reader );
				if( existingItr != readerItr->second.end() )
				{
					existingItr = readerItr->second.erase( existingItr );
				}
			}

		}

		void _RemoveAllReaders()
		{

			Kiwi::FreeMemory( m_eventReaders );

		}

	public:

		IEventQueueWriter( void (ReaderType :: *readerCallback)(const EventType&) )
		{
			m_readerCallback = readerCallback;
		}

		virtual ~IEventQueueWriter() {}

	};
}

#endif