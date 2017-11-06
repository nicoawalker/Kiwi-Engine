#ifndef _KIWI_IRENDEREVENTEMITTER_H_
#define _KIWI_IRENDEREVENTEMITTER_H_

#include "EventHandling.h"
#include "RenderEvent.h"

#include <mutex>

namespace Kiwi
{

	class IRenderEventEmitter :
		private Kiwi::IEventEmitter<RenderEvent>
	{
	protected:

		std::recursive_mutex m_renderEventEmitter_M;

	protected:

		void _EmitRenderEventQueue();

	public:

		IRenderEventEmitter() :
			Kiwi::IEventEmitter<RenderEvent>( std::initializer_list<std::wstring>( { L"PreRender", L"PostRender" } ) )
		{
		}
		~IRenderEventEmitter() {}

		void DisconnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver );

		void DisconnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::wstring& eventType );

		void DisconnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EnqueueRenderEvent( std::shared_ptr<RenderEvent> evt );

		void EmitRenderEvent( std::shared_ptr<RenderEvent> evt );

		void ConnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<RenderEvent> )> func );

		void ConnectRenderEventReceiver( IEventReceiver<RenderEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<RenderEvent> )> func );

		template<typename CallbackClass>
		void ConnectRenderEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<RenderEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_RenderEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectRenderEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<RenderEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_RenderEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif