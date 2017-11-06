#ifndef _KIWI_IEVENTRECEIVER_H_
#define _KIWI_IEVENTRECEIVER_H_

#include <set>
#include <memory>

#include <Windows.h>

namespace Kiwi
{

	class IEvent;

	template<typename> class IEventReceiver;

	template<typename EventType>
	class EventLinker
	{
	protected:

		IEventReceiver<EventType>* m_receiver;

		bool m_disabled;

	public:

		EventLinker( IEventReceiver<EventType>& receiver );

		void LinkEvent( std::shared_ptr<EventType>& evt );

		void Disable();

		bool IsDisabled()const;

	};

	template<typename EventType>
	class IEventReceiver
	{
		template<typename> friend class EventLinker;
		template<typename> friend class IEventDispatcher;
	private:

		std::shared_ptr<EventLinker<EventType>> m_eventLinker;

	private:

		std::shared_ptr<EventLinker<EventType>> _GetLinker();

	protected:

		virtual void _ReceiveEvent( std::shared_ptr<EventType> evt ) = 0;

	public:

		IEventReceiver();

		virtual ~IEventReceiver();

	};


	template<typename EventType>
	EventLinker<EventType>::EventLinker( IEventReceiver<EventType>& receiver )
	{
		m_receiver = &receiver;
		m_disabled = false;
	}

	template<typename EventType>
	void EventLinker<EventType>::LinkEvent( std::shared_ptr<EventType>& evt )
	{
		if( m_receiver != 0 )
		{
			m_receiver->_ReceiveEvent( evt );
		}
	}

	template<typename EventType>
	void EventLinker<EventType>::Disable()
	{
		m_disabled = true;
		m_receiver = 0;
	}

	template<typename EventType>
	bool EventLinker<EventType>::IsDisabled()const
	{
		return m_disabled;
	}

	template<typename EventType>
	std::shared_ptr<EventLinker<EventType>> IEventReceiver<EventType>::_GetLinker()
	{
		return m_eventLinker;
	}

	template<typename EventType>
	IEventReceiver<EventType>::IEventReceiver()
	{
		m_eventLinker = std::make_shared<EventLinker<EventType>>( *this );
	}

	template<typename EventType>
	IEventReceiver<EventType>::~IEventReceiver()
	{
		m_eventLinker->Disable();
	}

}

#endif