#ifndef _KIWI_ITRANSFORMEVENTBROADCASTER_H_
#define _KIWI_ITRANSFORMEVENTBROADCASTER_H_

#include "EventBroadcaster.h"

#include "ITransformEventListener.h"
#include "TransformEvent.h"

namespace Kiwi
{

	class ITransformEventBroadcaster :
		private Kiwi::EventBroadcaster<Kiwi::ITransformEventListener, Kiwi::TransformEvent>
	{
	public:

		ITransformEventBroadcaster() :
			EventBroadcaster<Kiwi::ITransformEventListener, Kiwi::TransformEvent>(&Kiwi::ITransformEventListener::_OnTransformEvent) {}

		virtual ~ITransformEventBroadcaster() {}

		void AddTransformListener( Kiwi::ITransformEventListener* listener )
		{
			this->_AddListener( listener );
		}

		void RemoveTransformListener( Kiwi::ITransformEventListener* listener )
		{
			this->_RemoveListener( listener );
		}

		void BroadcastTransformEvent( const Kiwi::TransformEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}

		void RemoveAllTransformListeners()
		{
			this->_RemoveAllListeners();
		}

	};
};

#endif