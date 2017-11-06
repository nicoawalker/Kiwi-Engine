#ifndef _KIWI_IASSETEVENTQUEUEWRITER_H_
#define _KIWI_IASSETEVENTQUEUEWRITER_H_

#include "IEventQueueWriter.h"
#include "IAssetEventQueueReader.h"

namespace Kiwi
{

	class AssetEvent;

	class IAssetEventQueueWriter :
		private Kiwi::IEventQueueWriter<Kiwi::IAssetEventQueueReader, Kiwi::AssetEvent>
	{
	public:

		IAssetEventQueueWriter() :
			IEventQueueWriter<Kiwi::IAssetEventQueueReader, Kiwi::AssetEvent>( &Kiwi::IAssetEventQueueReader::_QueueEvent ) {}

		virtual ~IAssetEventQueueWriter() {}

		void AddAssetEventQueueReader( Kiwi::IAssetEventQueueReader& reader, const std::vector<int>& eventIDs )
		{
			this->_AddReader( reader, eventIDs );
		}

		void WriteAssetEvent( const Kiwi::AssetEvent& evt )
		{
			this->_WriteEvent( evt );
		}

		/*removes the reader from recieving all events*/
		void RemoveAssetQueueEventReader( Kiwi::IAssetEventQueueReader& reader )
		{
			this->_RemoveReader( reader );
		}

		/*removes the reader from recieving the specified events*/
		void RemoveAssetQueueEventReader( Kiwi::IAssetEventQueueReader& reader, const std::vector<int>& eventIDs )
		{
			this->_RemoveReader( reader, eventIDs );
		}

		void RemoveAllAssetEventReaders()
		{
			this->_RemoveAllReaders();
		}

	};
}

#endif