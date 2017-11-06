#ifndef _KIWI_IASSETEVENTEMITTER_H_
#define _KIWI_IASSETEVENTEMITTER_H_

#include "EventHandling.h"
#include "AssetEvent.h"

#include <mutex>

namespace Kiwi
{

	class IAssetEventEmitter :
		private Kiwi::IEventEmitter<AssetEvent>
	{
	protected:

		std::recursive_mutex m_assetEventEmitter_M;

	protected:

		void _EmitAssetEventQueue();

	public:

		IAssetEventEmitter():
			Kiwi::IEventEmitter<AssetEvent>( std::initializer_list<std::wstring>( { L"AssetLoaded", L"AssetUnloaded", L"AssetFailed", L"GroupLoaded", L"GroupUnloaded" } ) )
		{
		}

		~IAssetEventEmitter() {}

		void DisconnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver );

		void DisconnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::wstring& eventType );

		void DisconnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::vector<std::wstring>& eventTypes );

		void EnqueueAssetEvent( std::shared_ptr<AssetEvent> evt );

		void EmitAssetEvent( std::shared_ptr<AssetEvent> evt );

		void ConnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<AssetEvent> )> func );

		void ConnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<AssetEvent> )> func );

		template<typename CallbackClass>
		void ConnectAssetEventReceiver( CallbackClass& receiver, const std::wstring& eventType, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<AssetEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventType, callbackFunction );

		}

		template<typename CallbackClass>
		void ConnectAssetEventReceiver( CallbackClass& receiver, const std::vector<std::wstring>& eventTypes, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<AssetEvent>) )
		{

			std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

			this->_Connect<CallbackClass>( receiver, eventTypes, callbackFunction );

		}

	};
}

#endif