#ifndef _KIWI_IASSETEVENTQUEUEREADER_H_
#define _KIWI_IASSETEVENTQUEUEREADER_H_

#include "AssetEvent.h"

#include <deque>
#include <vector>
#include <memory>

namespace Kiwi
{

	class IAssetEventQueueWriter;

	class IAssetEventQueueReader
	{
		friend class IAssetEventQueueWriter;
	private:

		void _QueueEvent( const AssetEvent& evt );

	protected:

		std::deque<AssetEvent> m_assetEventQueue;

		/*stores a list of event IDs that this reader wants to receive
		if this list is empty, the listener will receive all global events*/
		std::vector<int> m_assetEventIDs;

	protected:

		/*called by _PullNextAssetEvent for each asset event pulled*/
		virtual void _OnAssetEvent( const AssetEvent& e ) {}

		/*called immediately whenever a new asset event is written
		if this function returns true the event will NOT be placed in the event queue*/
		virtual bool _OnAssetEventImm( const AssetEvent& e ) {}

		/*pulls the next event in the event queue and sends it to _OnSceneEvent(const SceneQueueEvent& e)
		returns true if there are more events in the queue, and false if the queue is empty*/
		bool _PullNextAssetEvent();

	public:

		IAssetEventQueueReader() {}
		virtual ~IAssetEventQueueReader() {}

		/*sets the list of event IDs that this listener wants to receive
		if this list is not set, the listener will receive all global events*/
		void SetRegisteredAssetEvents( const std::vector<int> eventIDs ) { m_assetEventIDs = eventIDs; }

		const std::vector<int>& GetRegisteredEventIDs() { return m_assetEventIDs; }

	};
}

#endif