#ifndef _KIWI_RENDERWINDOW_H_
#define _KIWI_RENDERWINDOW_H_

#include "../Core/Vector2.h"
#include "..\Core\InputManager.h"

#include "../Events/IWindowEventEmitter.h"

#include <Windows.h>
#include <string>

namespace Kiwi
{

	class WindowEvent;
	class Renderer;

	class RenderWindow :
		public Kiwi::IWindowEventEmitter
	{
	protected:

		HWND m_hwnd;

		HINSTANCE m_hInst;

		std::wstring m_name;

		Kiwi::Vector2 m_position;
		Kiwi::Vector2 m_dimensions;

		InputManager m_inputManager;

		bool m_vsyncEnabled;
		bool m_isVisible;
		bool m_isFullscreen;

	public:

		RenderWindow( std::wstring windowName, DWORD style, DWORD exStyle, int x, int y,
					  int width, int height, bool vsyncEnabled, HWND parent, HMENU menu );
		virtual ~RenderWindow();

		void Shutdown();

		LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void Update();

		bool Show(int cmdShow);

		bool CenterWindow(HWND parent);

		void RestrictMouse(bool isRestricted);

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool IsFullscreen()const { return m_isFullscreen; }
		bool IsVisible()const { return m_isVisible; }
		bool GetVSyncEnabled()const { return m_vsyncEnabled; }
		HWND GetHandle()const { return m_hwnd; }
		HINSTANCE GetInstance()const { return m_hInst; }
		InputManager& GetInputManager() { return m_inputManager; }
		Kiwi::Vector2 GetPosition();
		Kiwi::Vector2 GetWindowSize();
		Kiwi::Vector2 GetClientSize();
		std::wstring GetName()const { return m_name; }

		//void BroadcastMouseEvent( const Kiwi::MouseEvent& evt );

	};
};

#endif