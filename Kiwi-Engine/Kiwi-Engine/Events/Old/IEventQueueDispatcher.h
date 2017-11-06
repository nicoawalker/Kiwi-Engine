#ifndef _KIWI_IEVENTQUEUEDISPATCHER_H_
#define _KIWI_IEVENTQUEUEDISPATCHER_H_

//#include "IEvent.h"
//
//#include <unordered_map>
//#include <string>
//#include <set>
//#include <deque>
//#include <memory>
//#include <mutex>
//
//namespace Kiwi
//{
//
//	class IEventReceiver;
//
//	class IEventQueueDispatcher
//	{
//		friend class IEventReceiver;
//	private:
//
//		std::unordered_map<std::wstring, std::set<IEventReceiver*>> m_receivers;
//
//		std::deque<std::shared_ptr<Kiwi::IEvent>> m_eventQueue;
//
//		std::mutex m_dispatcher_M;
//
//	public:
//
//		virtual ~IEventQueueDispatcher();
//
//		void DispatchEvents();
//
//		void LinkReceiver( const std::wstring eventType, IEventReceiver& receiver );
//
//		void UnlinkReceiver( const std::wstring eventType, IEventReceiver& receiver );
//
//		void PostEvent( Kiwi::IEvent& evt );
//
//	};
//}


#endif