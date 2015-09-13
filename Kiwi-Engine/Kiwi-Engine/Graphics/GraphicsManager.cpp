#include "GraphicsManager.h"

#include "../Core/Exception.h"

namespace Kiwi
{

	GraphicsManager::GraphicsManager()
	{
	}

	GraphicsManager::~GraphicsManager()
	{
	}

	Kiwi::Renderer* GraphicsManager::CreateRenderer( std::wstring name, std::wstring renderWindowName )
	{

		try
		{
			Kiwi::RenderWindow* renderWindow = this->FindRenderWindowWithName( renderWindowName );

			return this->CreateRenderer( name, renderWindow );

		} catch( Kiwi::Exception& e )
		{
			throw e;
		}

	}

	Kiwi::Renderer* GraphicsManager::CreateRenderer( std::wstring name, Kiwi::RenderWindow* renderWindow )
	{

		if( renderWindow != 0 )
		{
			Kiwi::Renderer* renderer = new Kiwi::Renderer( name, renderWindow );

			m_renderers.Add( name, renderer );

			return renderer;

		} else
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderer", L"Failed to create renderer '" + name + L"': invalid RenderWindow" );
		}

	}

	Kiwi::RenderWindow* GraphicsManager::CreateRenderWindow( std::wstring windowName, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, bool vsyncEnabled )
	{

		Kiwi::RenderWindow* renderWindow = new Kiwi::RenderWindow( windowName, WS_OVERLAPPEDWINDOW, 0, (int)position.x, (int)position.y, (int)dimensions.x, (int)dimensions.y, vsyncEnabled, NULL, NULL );

		m_renderWindows.Add( windowName, renderWindow );

		return renderWindow;

	}

	Kiwi::RenderWindow* GraphicsManager::CreateRenderWindow( std::wstring windowName, DWORD style, DWORD exStyle, const Kiwi::Vector2& position,
															 const Kiwi::Vector2& dimensions, bool vsyncEnabled, Kiwi::RenderWindow* parentWindow, HMENU menu )
	{

		HWND parentHWND = 0;
		if( parentWindow )
		{
			parentHWND = parentWindow->GetHandle();
		}

		Kiwi::RenderWindow* renderWindow = new Kiwi::RenderWindow( windowName, style, exStyle, (int)position.x, (int)position.y, (int)dimensions.x, (int)dimensions.y, vsyncEnabled, parentHWND, menu );

		m_renderWindows.Add( windowName, renderWindow );

		return renderWindow;

	}

}