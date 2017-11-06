#include "ISceneEventQueueReader.h"

namespace Kiwi
{

	void ISceneEventQueueReader::_QueueEvent( const SceneEvent& evt )
	{

		m_sceneEventQueue.push_back( evt );

	}

	bool ISceneEventQueueReader::_PullNextSceneEvent()
	{

		if( m_sceneEventQueue.size() == 0 ) return false;

		Kiwi::SceneEvent evt = m_sceneEventQueue.front();
		m_sceneEventQueue.pop_front();

		this->_OnSceneEvent( evt );

		return true;

	}

}