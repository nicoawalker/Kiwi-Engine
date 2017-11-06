//#include "IEventReceiver.h"
//#include "EventDispatcher.h"
//
//namespace Kiwi
//{
//
//	IEventReceiver::~IEventReceiver()
//	{
//
//		/*unregister the receiver from all dispatchers it is receiving from*/
//		m_disabled = true;
//
//	}
//
//	/*void IEventReceiver::_LinkDispatcher( EventDispatcher& dispatcher )
//	{
//
//		auto itr = m_dispatchers.find( &dispatcher );
//		if( itr == m_dispatchers.end() )
//		{
//			m_dispatchers.insert( &dispatcher );
//		}
//
//	}
//
//	void IEventReceiver::_UnlinkDispatcher( EventDispatcher& dispatcher )
//	{
//
//		auto itr = m_dispatchers.find( &dispatcher );
//		if( itr != m_dispatchers.end() )
//		{
//			m_dispatchers.erase( itr );
//		}
//
//	}*/
//
//}