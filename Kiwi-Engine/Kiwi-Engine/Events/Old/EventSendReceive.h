#ifndef _KIWI_EVENTSENDRECEIVE_H_
#define _KIWI_EVENTSENDRECEIVE_H_

#include "..\Core\Utilities.h"

#include <unordered_set>
#include <memory>
#include <mutex>

namespace Kiwi
{

	class IEvent;

	//template<typename> class IEventDispatcher;
	//template<typename> class IEventReceiver;

	template<typename EventType>
	class IEventReceiver
	{
		template<typename> friend class IEventDispatcher;
	private:

		std::unordered_set<IEventDispatcher<EventType>*> m_dispatchers;

	protected:

		void _LinkDispatcher( IEventDispatcher<EventType>& dispatcher );
		
		void _UnlinkDispatcher( IEventDispatcher<EventType>& dispatcher );

		virtual void _ReceiveEvent( std::shared_ptr<EventType> evt ) = 0;

	public:

		IEventReceiver(){}

		virtual ~IEventReceiver();

	};

	template<typename EventType>
	class IEventDispatcher
	{
		template<typename> friend class IEventReceiver;
	private:

		std::unordered_set<IEventReceiver<EventType>*> m_receivers;

		std::unordered_set<IEventReceiver<EventType>*> m_newReceiverBuffer;
		std::unordered_set<IEventReceiver<EventType>*> m_erasedReceiverBuffer;

		std::recursive_mutex m_dispatcher_M;

	private:

		void _AddNewReceivers();

		void _RemoveErasedReceivers();

	protected:

		void _RegisterReceiver( IEventReceiver<EventType>& receiver );

		void _UnregisterReceiver( IEventReceiver<EventType>& receiver );

		void _DispatchEvent( std::shared_ptr<EventType> evt );

	public:

		IEventDispatcher() {}
		virtual ~IEventDispatcher();

	};

	template<typename EventType>
	IEventReceiver<EventType>::~IEventReceiver()
	{
		for( auto* disp : m_dispatchers )
		{
			disp->_UnregisterReceiver( *this );
		}
	}

	template<typename EventType>
	void IEventReceiver<EventType>::_LinkDispatcher( IEventDispatcher<EventType>& dispatcher )
	{

		auto itr = m_dispatchers.find( &dispatcher );
		if( itr == m_dispatchers.end() )
		{
			m_dispatchers.insert( &dispatcher );
		}

	}

	template<typename EventType>
	void IEventReceiver<EventType>::_UnlinkDispatcher( IEventDispatcher<EventType>& dispatcher )
	{

		auto itr = m_dispatchers.find( &dispatcher );
		if( itr != m_dispatchers.end() )
		{
			m_dispatchers.erase( itr );
		}

	}

	template<typename EventType> 
	IEventDispatcher<EventType>::~IEventDispatcher() 
	{
	
		for( auto itr = m_receivers.begin(); itr != m_receivers.end(); itr++ )
		{
			(*itr)->_UnlinkDispatcher( *this );
		}
	
	}

	template<typename EventType>
	void IEventDispatcher<EventType>::_AddNewReceivers()
	{
		if( m_newReceiverBuffer.size() > 0 )
		{
			for( auto* recvr : m_newReceiverBuffer )
			{
				auto itr = m_receivers.find( recvr );
				if( itr == m_receivers.end() )
				{
					recvr->_LinkDispatcher( *this );
					m_receivers.insert( recvr );
				}
			}

			Kiwi::FreeMemory( m_newReceiverBuffer );
		}
	}

	template<typename EventType>
	void IEventDispatcher<EventType>::_RemoveErasedReceivers()
	{
		if( m_erasedReceiverBuffer.size() > 0 )
		{
			for( auto* recvr : m_erasedReceiverBuffer )
			{
				auto itr = m_receivers.find( recvr );
				if( itr != m_receivers.end() )
				{
					m_receivers.erase( itr );
				}
			}
			Kiwi::FreeMemory( m_erasedReceiverBuffer );
		}
	}

	template<typename EventType>
	void IEventDispatcher<EventType>::_RegisterReceiver( IEventReceiver<EventType>& receiver )
	{
		std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );

		OutputDebugString( L"Registering...\n" );

		auto itr = m_receivers.find( &receiver );
		if( itr == m_receivers.end() )
		{
			itr = m_newReceiverBuffer.find( &receiver );
			if( itr == m_newReceiverBuffer.end() )
			{
				OutputDebugString( L"Registered!\n" );
				m_newReceiverBuffer.insert( &receiver );
			}
		}
	}

	template<typename EventType>
	void IEventDispatcher<EventType>::_UnregisterReceiver( IEventReceiver<EventType>& receiver )
	{

		std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );

		OutputDebugString( L"Unregistering...\n" );

		auto itr = m_receivers.find( &receiver );
		if( itr != m_receivers.end() )
		{
			itr = m_erasedReceiverBuffer.find( &receiver );
			if( itr == m_erasedReceiverBuffer.end() )
			{
				OutputDebugString( L"Unregistered!\n" );
				receiver._UnlinkDispatcher( *this );
				m_erasedReceiverBuffer.insert( &receiver );
			}
		} else
		{
			itr = m_newReceiverBuffer.find( &receiver );
			if( itr != m_newReceiverBuffer.end() )
			{
				m_newReceiverBuffer.erase( itr );
			}
		}

	}

	template<typename EventType>
	void IEventDispatcher<EventType>::_DispatchEvent( std::shared_ptr<EventType> evt )
	{

		if( evt == 0 ) return;

		std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );

		this->_RemoveErasedReceivers();
		this->_AddNewReceivers();

		OutputDebugString( L"About to dispatch\n" );
		for( auto recvItr = m_receivers.begin(); recvItr != m_receivers.end(); recvItr++ )
		{
			(*recvItr)->_ReceiveEvent( evt );
			OutputDebugString( L"Dispatched\n" );
		}

	}



}

#endif