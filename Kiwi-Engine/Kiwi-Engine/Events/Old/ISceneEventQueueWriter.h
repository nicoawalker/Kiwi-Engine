#ifndef _KIWI_ISCENEEVENTQUEUEWRITER_H_
#define _KIWI_ISCENEEVENTQUEUEWRITER_H_

#include "IEventQueueWriter.h"
#include "ISceneEventQueueReader.h"

namespace Kiwi
{

	class SceneEvent;

	class ISceneEventQueueWriter :
		private Kiwi::IEventQueueWriter<Kiwi::ISceneEventQueueReader, Kiwi::SceneEvent>
	{
	public:

		ISceneEventQueueWriter() :
			IEventQueueWriter<Kiwi::ISceneEventQueueReader, Kiwi::SceneEvent>( &Kiwi::ISceneEventQueueReader::_QueueEvent ) {}

		virtual ~ISceneEventQueueWriter() {}

		void AddSceneEventQueueReader( Kiwi::ISceneEventQueueReader& reader, const std::vector<int>& eventIDs )
		{
			this->_AddReader( reader, eventIDs );
		}

		void WriteSceneEvent( const Kiwi::SceneEvent& evt )
		{
			this->_WriteEvent( evt );
		}

		/*removes the reader from recieving all events*/
		void RemoveSceneQueueEventReader( Kiwi::ISceneEventQueueReader& reader )
		{
			this->_RemoveReader( reader );
		}

		/*removes the reader from recieving the specified events*/
		void RemoveSceneQueueEventReader( Kiwi::ISceneEventQueueReader& reader, const std::vector<int>& eventIDs )
		{
			this->_RemoveReader( reader, eventIDs );
		}

		void RemoveAllSceneEventReaders()
		{
			this->_RemoveAllReaders();
		}

	};
}

#endif