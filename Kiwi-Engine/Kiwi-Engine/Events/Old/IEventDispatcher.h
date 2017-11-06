#ifndef _KIWI_IEVENTDISPATCHER_H_
#define _KIWI_IEVENTDISPATCHER_H_

#include "IEventReceiver.h"

#include "..\Core\Utilities.h"

#include <unordered_map>
#include <string>
#include <unordered_set>
#include <deque>
#include <memory>
#include <mutex>

namespace Kiwi
{

	class IEvent;

	template<typename EventType>
	class IEventDispatcher
	{
	private:

		std::unordered_set<std::shared_ptr<EventLinker<EventType>>> m_receivers;
		std::unordered_set<std::shared_ptr<EventLinker<EventType>>> m_newReceiverBuffer;
		std::unordered_set<std::shared_ptr<EventLinker<EventType>>> m_erasedReceiverBuffer;

		std::recursive_mutex m_dispatcher_M;

	private:

		void _AddNewReceivers()
		{
			if( m_newReceiverBuffer.size() > 0 )
			{
				for( auto& recvr : m_newReceiverBuffer )
				{
					auto itr = m_receivers.find( recvr );
					if( itr == m_receivers.end() )
					{
						m_receivers.insert( recvr );
					}
				}

				Kiwi::FreeMemory( m_newReceiverBuffer );
			}
		}

		void _RemoveErasedReceivers()
		{
			if( m_erasedReceiverBuffer.size() > 0 )
			{
				for( auto& recvr : m_erasedReceiverBuffer )
				{
					auto itr = m_receivers.find( recvr );
					if( itr != m_receivers.end() )
					{
						m_receivers.erase( itr );
						KIWIDEBUGSTRING( Kiwi::ToWString( L"Removed receiver! Current registered receivers: " + Kiwi::ToWString( m_receivers.size() ) + L"\n" ).c_str() );
					}
				}
				Kiwi::FreeMemory( m_erasedReceiverBuffer );
			}
		}

	protected:

		void _RegisterReceiver( IEventReceiver<EventType>& receiver )
		{
			std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );

			auto recvItr = m_receivers.find( receiver._GetLinker() );
			if( recvItr == m_receivers.end() )
			{
				auto newItr = m_newReceiverBuffer.find( receiver._GetLinker() );
				if( newItr == m_newReceiverBuffer.end() )
				{
					m_newReceiverBuffer.insert( receiver._GetLinker() );
				}
			}
		}

		void _UnregisterReceiver( IEventReceiver<EventType>& receiver )
		{

			std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );

			auto recvItr = m_receivers.find( receiver._GetLinker() );
			if( recvItr != m_receivers.end() )
			{
				auto eraseItr = m_erasedReceiverBuffer.find( receiver._GetLinker() );
				if( eraseItr == m_erasedReceiverBuffer.end() )
				{
					m_erasedReceiverBuffer.insert( receiver._GetLinker() );
				}
			}

		}

		void _DispatchEvent( std::shared_ptr<EventType> evt )
		{

			if( evt == 0 ) return;

			std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );

			this->_RemoveErasedReceivers();
			this->_AddNewReceivers();

			for( auto recvItr = m_receivers.begin(); recvItr != m_receivers.end();)
			{
				if( (*recvItr)->IsDisabled() )
				{
					recvItr = m_receivers.erase( recvItr );
					KIWIDEBUGSTRING( Kiwi::ToWString( L"Removed disabled receiver! Current registered receivers: " + Kiwi::ToWString( m_receivers.size() ) + L"\n" ).c_str() );
					continue;
				}
				(*recvItr)->LinkEvent( evt );
				recvItr++;
			}

		}

	public:

		IEventDispatcher(){ }
		virtual ~IEventDispatcher() = 0;

	};

	template<typename EventType> IEventDispatcher<EventType>::~IEventDispatcher() {}
}

#endif