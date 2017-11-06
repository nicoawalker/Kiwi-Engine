#include "IAssetEventEmitter.h"

#include "../Types.h"

namespace Kiwi
{

	void IAssetEventEmitter::_EmitAssetEventQueue()
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

		this->_EmitEventQueue();
	}

	void IAssetEventEmitter::DisconnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

		this->_Disconnect( receiver );
	}

	void IAssetEventEmitter::DisconnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::wstring& eventType )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );
		
		this->_Disconnect( receiver, eventType );
	}

	void IAssetEventEmitter::DisconnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::vector<std::wstring>& eventTypes )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

		this->_Disconnect( receiver, eventTypes );
	}

	void IAssetEventEmitter::EnqueueAssetEvent( std::shared_ptr<AssetEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

		this->_EnqueueEvent( evt->eventCategory, evt );
	}

	void IAssetEventEmitter::EmitAssetEvent( std::shared_ptr<AssetEvent> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

		this->_EmitEvent( evt->eventCategory, evt );
	}

	void IAssetEventEmitter::ConnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::wstring& eventType, std::function < Kiwi::EventResponse( std::shared_ptr<AssetEvent> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

		this->_Connect( receiver, eventType, func );
	}

	void IAssetEventEmitter::ConnectAssetEventReceiver( IEventReceiver<AssetEvent>& receiver, const std::vector<std::wstring>& eventTypes, std::function < Kiwi::EventResponse( std::shared_ptr<AssetEvent> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_assetEventEmitter_M );

		this->_Connect( receiver, eventTypes, func );
	}

}