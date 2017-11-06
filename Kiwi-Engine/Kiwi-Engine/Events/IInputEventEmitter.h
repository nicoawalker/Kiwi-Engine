#ifndef _KIWI_IINPUTEVENTEMITTER_H_
#define _KIWI_IINPUTEVENTEMITTER_H_

#include "EventHandling.h"
#include "InputEvent.h"

#include <mutex>

namespace Kiwi
{

	class IInputEventEmitter :
		private Kiwi::IEventEmitter<InputEvent>
	{
	protected:

		std::recursive_mutex m_inputEventEmitter_M;

	protected:

		void _EmitInputEventQueue();

	public:

		IInputEventEmitter() :
			Kiwi::IEventEmitter<InputEvent>( std::initializer_list<std::wstring>( { L"KeyUp", L"KeyHeld", L"KeyReleased", L"KeyDown", L"MouseUp", L"MouseDown", L"MouseHeld", L"MouseReleased" } ) )
		{
		}
		~IInputEventEmitter() {}

		void AddInputCategory( const std::wstring& cat )
		{
			this->_AddCategory( cat );
		}

		void DisconnectInputEventReceiver( IEventReceiver<InputEvent>& receiver );

		void DisconnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::wstring& eventType );

		void DisconnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EnqueueInputEvent( std::shared_ptr<InputEvent> evt );

		void EmitInputEvent( std::shared_ptr<InputEvent> evt );

		void ConnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<InputEvent> )> func );

		void ConnectInputEventReceiver( IEventReceiver<InputEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<InputEvent> )> func );

		template<typename CallbackClass>
		void ConnectInputEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<InputEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectInputEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<InputEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_inputEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif