#include "ISceneEventEmitter.h"

namespace Kiwi
{

	void ISceneEventEmitter::_EmitSceneEventQueue()
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_EmitEventQueue();
	}

	void ISceneEventEmitter::DisconnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_Disconnect( receiver );
	}

	void ISceneEventEmitter::DisconnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::wstring& eventCategory )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_Disconnect( receiver, eventCategory );
	}

	void ISceneEventEmitter::DisconnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::vector<std::wstring>& eventCategories )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_Disconnect( receiver, eventCategories );
	}

	void ISceneEventEmitter::EmitSceneEvent( std::shared_ptr<SceneEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void ISceneEventEmitter::EnqueueSceneEvent( std::shared_ptr<SceneEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_EnqueueEvent( evt->eventCategory, evt );
	}

	void ISceneEventEmitter::ConnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::wstring& eventCategory, std::function < Kiwi::EventResponse( std::shared_ptr<SceneEvent> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_Connect( receiver, eventCategory, func );
	}

	void ISceneEventEmitter::ConnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::vector<std::wstring>& eventCategories, std::function < Kiwi::EventResponse( std::shared_ptr<SceneEvent> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

		this->_Connect( receiver, eventCategories, func );
	}

}