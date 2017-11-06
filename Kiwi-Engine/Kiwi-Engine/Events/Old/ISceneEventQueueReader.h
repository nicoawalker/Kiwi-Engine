#ifndef _KIWI_ISCENEEVENTQUEUEREADER_H_
#define _KIWI_ISCENEEVENTQUEUEREADER_H_

#include "SceneEvent.h"

#include <deque>
#include <vector>
#include <memory>

namespace Kiwi
{

	class ISceneEventQueueWriter;

	class ISceneEventQueueReader
	{
		friend class ISceneEventQueueWriter;
	private:

		void _QueueEvent( const SceneEvent& evt );

	protected:

		std::deque<SceneEvent> m_sceneEventQueue;

		/*stores a list of event IDs that this listener wants to receive
		if this list is empty, the listener will receive all global events*/
		std::vector<int> m_sceneEventIDs;

	protected:

		virtual void _OnSceneEvent( const SceneEvent& e ) {}

		/*pulls the next event in the event queue and sends it to _OnSceneEvent(const SceneQueueEvent& e)
		returns true if there are more events in the queue, and false if the queue is empty*/
		bool _PullNextSceneEvent();

	public:

		ISceneEventQueueReader() {}
		virtual ~ISceneEventQueueReader() {}

		/*sets the list of event IDs that this listener wants to receive
		if this list is not set, the listener will receive all global events*/
		void SetRegisteredEventIDs( const std::vector<int> eventIDs ) { m_sceneEventIDs = eventIDs; }

		const std::vector<int>& GetRegisteredEventIDs() { return m_sceneEventIDs; }

	};
}

#endif