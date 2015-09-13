#ifndef _KIWI_WINDOWEVENT_H_
#define _KIWI_WINDOWEVENT_H_

#include "..\Core\Event.h"

#include <Windows.h>

namespace Kiwi
{

	class RenderWindow;

	class WindowEvent : public Kiwi::Event<Kiwi::RenderWindow>
	{
	protected:

		unsigned int m_windowMsg;

		WPARAM m_wParam;
		LPARAM m_lParam;

	public:

		WindowEvent(Kiwi::RenderWindow* source, unsigned int windowMsg, WPARAM wParam = 0, LPARAM lParam = 0) : 
			Event(source),
			m_windowMsg(windowMsg), m_wParam(wParam), m_lParam(lParam) {}

		virtual ~WindowEvent() {}

		void SetWindowMessage(unsigned int msg) { m_windowMsg = msg; }
		void SetWParam(WPARAM wParam) { m_wParam = wParam; }
		void SetLParam(LPARAM lParam) { m_lParam = lParam; }

		unsigned int GetWindowMessage()const { return m_windowMsg; }
		WPARAM GetWParam()const { return m_wParam; }
		LPARAM GetLParam()const { return m_lParam; }

	};
};

#endif