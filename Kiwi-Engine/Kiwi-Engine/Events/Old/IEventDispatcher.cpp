//#include "EventDispatcher.h"
//#include "IEventReceiver.h"
//#include "IEvent.h"
//
//#include "..\Core\Utilities.h"
//
//namespace Kiwi
//{
//
//	EventDispatcher::~EventDispatcher()
//	{
//	}
//
//	void EventDispatcher::_AddNewReceivers()
//	{
//
//		if( m_newReceiverBuffer.size() > 0 )
//		{
//			for( auto itr = m_newReceiverBuffer.begin(); itr != m_newReceiverBuffer.end(); itr++ )
//			{
//
//			}
//		}
//
//	}
//
//	void EventDispatcher::RegisterReceiver( std::shared_ptr<IEventReceiver> receiver )
//	{
//
//		std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );
//
//		OutputDebugString( L"Registering...\n" );
//		auto typeItr = m_receivers.find( receiver->GetEventType() );
//		if( typeItr != m_receivers.end() )
//		{
//			auto recvItr = typeItr->second.find( receiver );
//			if( recvItr == typeItr->second.end() )
//			{
//				recvItr = m_newReceiverBuffer.find( receiver );
//				if( recvItr == m_newReceiverBuffer.end() )
//				{
//					OutputDebugString( L"Registered!\n" );
//					m_newReceiverBuffer.insert( receiver );
//				}
//			}
//		}
//
//	}
//
//	void EventDispatcher::UnregisterReceiver( std::shared_ptr<IEventReceiver> receiver )
//	{
//
//		receiver->m_disabled = true;
//		/*std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );
//
//		for( auto& typeSet : m_receivers )
//		{
//			auto recvItr = typeSet.second.find( &receiver );
//			if( recvItr != typeSet.second.end() )
//			{
//				receiver._UnlinkDispatcher( *this );
//				typeSet.second.erase( recvItr );
//				if( m_dispatchingEvents )
//				{
//					m_receiversChanged = true;
//				}
//			}
//		}*/
//
//	}
//
//	void EventDispatcher::DispatchEvent( std::shared_ptr<Kiwi::IEvent> evt )
//	{
//		if( evt == 0 ) return;
//
//		std::lock_guard<std::recursive_mutex> lock( m_dispatcher_M );
//
//		m_dispatchingEvents = true;
//
//		OutputDebugString( L"About to dispatch\n" );
//		auto typeItr = m_receivers.find( evt->GetType() );
//		if( typeItr != m_receivers.end() )
//		{
//			for( auto recvrItr = typeItr->second.begin(); recvrItr != typeItr->second.end(); )
//			{
//				OutputDebugString( L"Dispatched\n" );
//				Kiwi::IEventReceiver* recvr = *recvrItr;
//				auto nextItr = recvrItr+1;
//				recvr->_ReceiveEvent( evt );
//
//				if( m_receiversChanged )
//				{/*if a receiver was added or removed, update the iterators*/
//					typeItr = m_receivers.find( evt->GetType() );
//					if( typeItr == m_receivers.end() )
//					{
//						return;
//					}
//					recvrItr = typeItr->second.find( recvr );
//					m_receiversChanged = false;
//				}
//				recvrItr++;
//			}
//		}
//
//		m_dispatchingEvents = false;
//
//	}
//
//}