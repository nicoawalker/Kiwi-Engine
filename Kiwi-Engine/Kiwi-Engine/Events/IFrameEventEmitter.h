#ifndef _KIWI_IFRAMEEVENTEMITTER_H_
#define _KIWI_IFRAMEEVENTEMITTER_H_

#include "EventHandling.h"
#include "FrameEvent.h"

#include <mutex>

namespace Kiwi
{

	class IFrameEventEmitter :
		private Kiwi::IEventEmitter<FrameEvent>
	{
	protected:

		std::recursive_mutex m_frameEventEmitter_M;

	public:

		IFrameEventEmitter() :
			Kiwi::IEventEmitter<FrameEvent>( std::initializer_list<std::wstring>( { L"" } ) )
		{
		}
		~IFrameEventEmitter() {}

		void DisconnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver );

		void DisconnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::wstring& eventType );

		void DisconnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EmitFrameEvent( std::shared_ptr<FrameEvent> evt );

		void ConnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<FrameEvent> )> func );

		void ConnectFrameEventReceiver( IEventReceiver<FrameEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<FrameEvent> )> func );

		template<typename CallbackClass>
		void ConnectFrameEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<FrameEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectFrameEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<FrameEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_frameEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif