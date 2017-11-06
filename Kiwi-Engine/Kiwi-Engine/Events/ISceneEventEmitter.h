#ifndef _KIWI_ISCENEEVENTEMITTER_H_
#define _KIWI_ISCENEEVENTEMITTER_H_

#include "EventHandling.h"
#include "SceneEvent.h"

#include <mutex>

namespace Kiwi
{

	class ISceneEventEmitter :
		private Kiwi::IEventEmitter<SceneEvent>
	{
	protected:

		std::recursive_mutex m_sceneEventEmitter_M_;

	protected:

		void _EmitSceneEventQueue();

	public:

		ISceneEventEmitter() :
			Kiwi::IEventEmitter<SceneEvent>( std::initializer_list<std::wstring>( { L"", L"SceneStarted", L"PreRender", L"RenderStart", L"RenderEnd", L"PostRender" } ) )
		{
		}
		~ISceneEventEmitter() {}

		void DisconnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver );

		void DisconnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::wstring& eventType );

		void DisconnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EmitSceneEvent( std::shared_ptr<SceneEvent> evt );

		void EnqueueSceneEvent( std::shared_ptr<SceneEvent> evt );

		void ConnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<SceneEvent> )> func );

		void ConnectSceneEventReceiver( IEventReceiver<SceneEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<SceneEvent> )> func );

		template<typename CallbackClass>
		void ConnectSceneEventReceiver( CallbackClass& receiver, const std::wstring& eventCategory, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<SceneEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

			this->_Connect<CallbackClass>( receiver, eventCategory, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectSceneEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventCategories, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<SceneEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_sceneEventEmitter_M_ );

			this->_Connect<CallbackClass>( receiver, eventCategories, callbackFunction );

		}

	};
}

#endif