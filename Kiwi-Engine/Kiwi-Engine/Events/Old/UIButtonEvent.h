#ifndef _KIWI_UIBUTTONEVENT_H_
#define _KIWI_UIBUTTONEVENT_H_

#include "Event.h"

#include "../Core/Vector2d.h"

namespace Kiwi
{

	class UIButton;

	class UIButtonEvent: 
		public Kiwi::Event<Kiwi::UIButton>
	{
	public:

		enum BUTTONEVENT_TYPE { BUTTON_PRESS, BUTTON_RELEASE, BUTTON_HELD };

	protected:

		BUTTONEVENT_TYPE m_eventType;

		//position of the mouse when the event occured
		Kiwi::Vector2d m_mousePos;

	public:

		UIButtonEvent( Kiwi::UIButton* source, BUTTONEVENT_TYPE eventType, const Kiwi::Vector2d& mousePos ) :
			Event(source), m_eventType(eventType), m_mousePos(mousePos) {}

		virtual ~UIButtonEvent() {}

		BUTTONEVENT_TYPE GetEventType()const { return m_eventType; }

		const Kiwi::Vector2d& GetMousePos()const { return m_mousePos; }

	};
};

#endif