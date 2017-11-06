#include "UIButtonEventListener.h"

namespace Kiwi
{

	void UIButtonEventListener::OnButtonEvent( const Kiwi::UIButtonEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case UIButtonEvent::BUTTON_RELEASE:
				{
					this->_OnButtonRelease( evt );
					break;
				}
			case UIButtonEvent::BUTTON_HELD:
				{
					this->_OnButtonHeld( evt );
					break;
				}
			case UIButtonEvent::BUTTON_PRESS:
				{
					this->_OnButtonPress( evt );
					break;
				}
			default: return;
		}

	}

}