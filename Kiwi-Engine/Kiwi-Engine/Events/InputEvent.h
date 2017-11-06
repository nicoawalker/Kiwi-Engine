#ifndef _KIWI_INPUTEVENT_H_
#define _KIWI_INPUTEVENT_H_

#include "IEvent.h"

#include "../Types.h"

#include "..\Core\Vector2d.h"

namespace Kiwi
{

	class RenderWindow;

	class InputEvent :
		public Kiwi::IEvent<Kiwi::RenderWindow>
	{
	public:

		std::wstring mappedInput;

	public:

		InputEvent( Kiwi::RenderWindow* source, const std::wstring& translatedInput ) :
			Kiwi::IEvent<RenderWindow>( source, translatedInput ),
			mappedInput(translatedInput) {}

	};

	struct MouseEvent :
		public InputEvent
	{
		short wheelDelta; //amount the scroll wheel was moved
		Kiwi::Vector2d delta; //change in position since last frame
		Kiwi::Vector2d pos; //current position of the mouse, relative to the center of the window

		MouseEvent( Kiwi::RenderWindow* source, const std::wstring& translatedInput, const Kiwi::Vector2d& mousePos, const Kiwi::Vector2d& mouseDeltaPos, const short mouseWheelDelta = 0 ):
			InputEvent(source, translatedInput),
			wheelDelta(mouseWheelDelta), delta(mouseDeltaPos), pos(mousePos){ }

	};

}

#endif