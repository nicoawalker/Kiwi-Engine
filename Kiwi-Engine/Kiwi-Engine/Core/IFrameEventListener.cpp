#include "IFrameEventListener.h"

namespace Kiwi
{

	void IFrameEventListener::OnFrameEvent( const Kiwi::FrameEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case Kiwi::FrameEvent::EventType::UNTIMED_EVENT:
				{
					this->OnUpdate( evt );
					break;
				}
			case Kiwi::FrameEvent::EventType::TIMED_EVENT:
				{
					this->OnFixedUpdate( evt );
					break;
				}
			default: break;
		}

	}

}