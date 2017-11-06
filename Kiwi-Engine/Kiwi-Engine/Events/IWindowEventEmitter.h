#ifndef _KIWI_IWINDOWEVENTEMITTER_H_
#define _KIWI_IWINDOWEVENTEMITTER_H_

#include "EventHandling.h"
#include "WindowEvent.h"

#include <mutex>

namespace Kiwi
{

	class IWindowEventEmitter :
		private Kiwi::IEventEmitter<WindowEvent>
	{
	protected:

		std::recursive_mutex m_windowEventEmitter_M;

	public:

		IWindowEventEmitter() :
			Kiwi::IEventEmitter<WindowEvent>( std::initializer_list<std::wstring>( { L"Move", L"Message" } ) )
		{
		}
		~IWindowEventEmitter() {}

		void DisconnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver );

		void DisconnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::wstring& eventType );

		void DisconnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EmitWindowEvent( std::shared_ptr<WindowEvent> evt );

		void ConnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<WindowEvent> )> func );

		void ConnectWindowEventReceiver( IEventReceiver<WindowEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<WindowEvent> )> func );

		template<typename CallbackClass>
		void ConnectWindowEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<WindowEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectWindowEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<WindowEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_windowEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif