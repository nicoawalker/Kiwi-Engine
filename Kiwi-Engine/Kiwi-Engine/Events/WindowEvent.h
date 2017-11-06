#ifndef _KIWI_WINDOWEVENT_H_
#define _KIWI_WINDOWEVENT_H_

#include "IEvent.h"

#include "..\Core\Vector2d.h"

#include <Windows.h>

namespace Kiwi
{

	class RenderWindow;

	class WindowEvent: 
		public Kiwi::IEvent<Kiwi::RenderWindow>
	{
	public:

		WindowEvent(Kiwi::RenderWindow& source, const std::wstring windowEventType) : 
			IEvent<Kiwi::RenderWindow>( &source, windowEventType ) {}

		virtual ~WindowEvent() {}

	};

	struct WindowMessageEvent :
		public WindowEvent
	{
		unsigned int windowMsg;
		WPARAM wParam;
		LPARAM lParam;

		WindowMessageEvent( Kiwi::RenderWindow& source, unsigned int msg, WPARAM wparam = 0, LPARAM lparam = 0 ):
			WindowEvent(source, L"Message"), windowMsg(msg), wParam(wparam), lParam(lparam){ }
	};

	struct WindowMoveEvent :
		public WindowEvent
	{
		Kiwi::Vector2d position;

		WindowMoveEvent( Kiwi::RenderWindow& source, const Kiwi::Vector2d& newPosition ):
			WindowEvent(source, L"Move"), position(newPosition){ }
	};

};

#endif