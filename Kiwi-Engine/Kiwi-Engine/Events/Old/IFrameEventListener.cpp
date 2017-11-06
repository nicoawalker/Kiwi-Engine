#include "IFrameEventListener.h"

namespace Kiwi
{

	void IFrameEventListener::_OnFrameEvent( const Kiwi::FrameEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case Kiwi::FrameEvent::EventType::UPDATE:
				{
					this->_OnUpdate();
					break;
				}
			case Kiwi::FrameEvent::EventType::FIXED_UPDATE:
				{
					this->_OnFixedUpdate();
					break;
				}
			case Kiwi::FrameEvent::EventType::RENDER_START:
				{
					this->_OnRenderStart();
					break;
				}
			case Kiwi::FrameEvent::EventType::RENDER_END:
				{
					this->_OnRenderEnd();
					break;
				}
			default: break;
		}

	}

}