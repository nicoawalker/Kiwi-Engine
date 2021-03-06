#include "RenderWindow.h"

#include "..\Core\Exception.h"

namespace Kiwi
{

	RenderWindow::RenderWindow( std::wstring windowName, DWORD style, DWORD exStyle, int x, int y,
								int width, int height, bool vsyncEnabled, HWND parent, HMENU menu ) :
		m_inputManager( *this )
	{

		m_hwnd = 0;
		m_isVisible = false;
		m_isFullscreen = false;

		//first initialize the window parameters and create the window
		m_name = windowName;
		m_vsyncEnabled = vsyncEnabled;

		std::wstring className = L"KiwiRenderWindow_" + windowName;

		//initialize the window properties
		WNDCLASSEX wc;
		ZeroMemory( &wc, sizeof( WNDCLASSEX ) );

		m_hInst = GetModuleHandle( NULL );

		wc.cbSize = sizeof( WNDCLASSEX );
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = Kiwi::RenderWindow::WindowProc;
		wc.hInstance = m_hInst;
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.lpszClassName = className.c_str();

		//register the window's class
		RegisterClassEx( &wc );
		if( &wc == NULL )
		{
			//registration failed
			throw(Kiwi::Exception( L"RenderWindow::Intialize", L"Registration of the main window failed!" ));
		}

		//convert the passed width and height so that the client area is the specified size
		RECT windowArea = { 0, 0, width, height };
		AdjustWindowRectEx( &windowArea, style, 0, exStyle );

		m_hwnd = CreateWindowEx( exStyle, className.c_str(), m_name.c_str(), style, x, y,
								 (windowArea.right - windowArea.left), (windowArea.bottom - windowArea.top), parent, menu, m_hInst, this );

		if( !m_hwnd )
		{
			//window creation failed
			throw Kiwi::Exception( L"RenderWindow::Intialize", L"["+windowName+L"] Call to CreateWindowEx failed" );
		}

		m_position.Set( (float)x, (float)y );
		m_dimensions.Set( (float)width, (float)height );

		/*initialize raw input for this window*/
		m_inputManager.InitializeRawInput();

	}

	RenderWindow::~RenderWindow()
	{

	}

	void RenderWindow::Shutdown()
	{

		//PostQuitMessage( 0 );

	}

	LRESULT CALLBACK RenderWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//this is a static method so the first thing to do is get a pointer to the correct instance of the class

		//pointer to an instance of the window
		Kiwi::RenderWindow* pThis = NULL;

		if(msg == WM_NCCREATE) //window is just being created, store the pointer to be retrieved later
		{
			//retrieve the creation parameters unique to this window, and store them in the window
			CREATESTRUCT* create = (CREATESTRUCT*)lParam; //cast lParam to createstruct to get the value passed from CreateWindowEx
			pThis = (Kiwi::RenderWindow*)create->lpCreateParams; //get the actual data, and cast it to the correct BaseWindow type
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis); //store the data within the window

			pThis->m_hwnd = hwnd;

		}else //get pointer to the instance
		{
			pThis = (Kiwi::RenderWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if(pThis)
		{
			//send the message to the instance so that it can handle it
			return pThis->HandleMessage(msg, wParam, lParam);
		}else
		{
			//no instance, so do the default action
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

	}

	LRESULT RenderWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{

		switch( msg )
		{
			case WM_INPUT:
				{
					/*get the raw input information*/
					RAWINPUT InputData;

					UINT DataSize = sizeof( RAWINPUT );
					GetRawInputData( (HRAWINPUT)lParam,
									 RID_INPUT,
									 &InputData,
									 &DataSize,
									 sizeof( RAWINPUTHEADER ) );

					m_inputManager.ProcessInput( InputData );

					break;
				}
			default: break;
		}

		//send this event to the listeners
		this->EmitWindowEvent( std::make_shared<Kiwi::WindowMessageEvent>( *this, msg, wParam, lParam ) );

		return DefWindowProc(m_hwnd, msg, wParam, lParam);
	}

	void RenderWindow::Update()
	{

		m_inputManager.Update();

	}

	Kiwi::Vector2 RenderWindow::GetPosition()
	{

		RECT mainWinRect;
		GetWindowRect(m_hwnd, &mainWinRect);

		Kiwi::Vector2 pos;
		pos.x = (float)mainWinRect.left;
		pos.y = (float)mainWinRect.top;
		
		return pos;

	}

	Kiwi::Vector2 RenderWindow::GetWindowSize()
	{

		RECT mainWinRect;
		GetWindowRect(m_hwnd, &mainWinRect);

		Kiwi::Vector2 size;
		size.x = (float)(mainWinRect.right - mainWinRect.left);
		size.y = (float)(mainWinRect.bottom - mainWinRect.top);

		return size;

	}

	Kiwi::Vector2 RenderWindow::GetClientSize()
	{

		RECT clientSize;
		GetClientRect(m_hwnd, &clientSize);

		Kiwi::Vector2 size;
		size.x = (float)clientSize.right;
		size.y = (float)clientSize.bottom;

		return size;

	}

	bool RenderWindow::Show(int cmdShow)
	{

		if(cmdShow == SW_SHOW)
		{
			m_isVisible = true;
		}else
		{
			m_isVisible = false;
		}

		return (ShowWindow( m_hwnd, cmdShow )) ? true : false;
	}

	bool RenderWindow::CenterWindow(HWND parent)
	{

		if(parent == NULL) return false;

		RECT rc;
		GetClientRect(parent, &rc);
		rc.left = (rc.right / 2) - (long)(m_dimensions.x/2.0f);
		rc.top = (rc.bottom / 2) - (long)(m_dimensions.y/2.0f);

		if(!SetWindowPos(m_hwnd, HWND_TOP, rc.left, rc.top, (int)m_dimensions.x, (int)m_dimensions.y, SWP_NOZORDER | SWP_NOSIZE))
		{
			return false;
		}

		m_position.Set((float)rc.left, (float)rc.top);

		this->EmitWindowEvent( std::make_shared<Kiwi::WindowMoveEvent>( *this, Kiwi::Vector2d( rc.left, rc.top ) ) );

		return true;

	}

	void RenderWindow::RestrictMouse(bool isRestricted)
	{

		if(isRestricted)
		{

			//get the window client area
			RECT rc;
			GetClientRect(m_hwnd, &rc);

			// Convert the client area to screen coordinates.
			POINT pt = { rc.left, rc.top };
			POINT pt2 = { rc.right, rc.bottom };
			ClientToScreen(m_hwnd, &pt);
			ClientToScreen(m_hwnd, &pt2);
			SetRect(&rc, pt.x + 1, pt.y + 1, pt2.x - 1, pt2.y - 1);

			// Confine the cursor.
			ClipCursor(&rc);

		}else
		{
			ClipCursor(NULL);
		}

	}

};