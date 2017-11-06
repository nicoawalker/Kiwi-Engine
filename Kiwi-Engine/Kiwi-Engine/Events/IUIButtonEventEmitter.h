#ifndef _KIWI_IUIBUTTONEVENTEMITTER_H_
#define _KIWI_IUIBUTTONEVENTEMITTER_H_

#include "EventHandling.h"
#include "UIButtonEvent.h"

#include <mutex>

namespace Kiwi
{

	class IUIButtonEventEmitter :
		private Kiwi::IEventEmitter<UIButtonEvent>
	{
	protected:

		std::recursive_mutex m_uibuttonEventEmitter_M;

	protected:

		void _EmitUIButtonEventQueue();

	public:

		IUIButtonEventEmitter() :
			Kiwi::IEventEmitter<UIButtonEvent>( std::initializer_list<std::wstring>( { L"ButtonPressed", L"ButtonReleased", L"ButtonHeld" } ) )
		{
		}

		~IUIButtonEventEmitter() {}

		void DisconnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver );

		void DisconnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::wstring& eventType );

		void DisconnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EnqueueUIButtonEvent( std::shared_ptr<UIButtonEvent> evt );

		void EmitUIButtonEvent( std::shared_ptr<UIButtonEvent> evt );

		void ConnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<UIButtonEvent> )> func );

		void ConnectUIButtonEventReceiver( IEventReceiver<UIButtonEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<UIButtonEvent> )> func );

		template<typename CallbackClass>
		void ConnectUIButtonEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<UIButtonEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectUIButtonEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<UIButtonEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_uibuttonEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif