#ifndef _KIWI_ITRANSFORMEVENTEMITTER_H_
#define _KIWI_ITRANSFORMEVENTEMITTER_H_

#include "EventHandling.h"
#include "TransformEvent.h"

#include <mutex>

namespace Kiwi
{

	class ITransformEventEmitter :
		private Kiwi::IEventEmitter<TransformEvent>
	{
	protected:

		std::recursive_mutex m_transformEventEmitter_M;

	public:

		ITransformEventEmitter() :
			Kiwi::IEventEmitter<TransformEvent>( std::initializer_list<std::wstring>( { L"Rotation", L"Translation" } ) )
		{
		}
		~ITransformEventEmitter() {}

		void DisconnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver );

		void DisconnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::wstring& eventType );

		void DisconnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EmitTransformEvent( std::shared_ptr<TransformEvent> evt );

		void ConnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<TransformEvent> )> func );

		void ConnectTransformEventReceiver( IEventReceiver<TransformEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<TransformEvent> )> func );

		template<typename CallbackClass>
		void ConnectTransformEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<TransformEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectTransformEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<TransformEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_transformEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif