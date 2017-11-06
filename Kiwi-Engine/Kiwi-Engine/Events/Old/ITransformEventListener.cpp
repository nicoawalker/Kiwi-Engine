#include "ITransformEventListener.h"

#include "../Core/Utilities.h"

namespace Kiwi
{

	void ITransformEventListener::_OnTransformEvent( const Kiwi::TransformEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case TransformEvent::TRANSFORM_TRANSLATION:
				{
					this->_OnTransformTranslate( evt );
					break;
				}
			case TransformEvent::TRANSFORM_ROTATION:
				{
					this->_OnTransformRotate( evt );
					break;
				}
			default: return;
		}

	}

}