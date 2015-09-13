#ifndef _KIWI_GRAPHICSMANAGER_H_
#define _KIWI_GRAPHICSMANAGER_H_

#include "Renderer.h"
#include "RenderWindow.h"

#include "../Core/ComponentContainer.h"

#include <string>

namespace Kiwi
{
	/*this class is responsible for managing all of the graphical assets, resources
	and classes used by the engine*/
	class GraphicsManager
	{
	protected:

		Kiwi::ComponentContainer<std::wstring, Kiwi::Renderer> m_renderers;
		Kiwi::ComponentContainer<std::wstring, Kiwi::RenderWindow> m_renderWindows;

	public:

		GraphicsManager();
		~GraphicsManager();

		Kiwi::Renderer* CreateRenderer( std::wstring name, std::wstring renderWindowName );
		Kiwi::Renderer* CreateRenderer( std::wstring name, Kiwi::RenderWindow* renderWindow );

		Kiwi::Renderer* FindRendererWithName( std::wstring name ) { return m_renderers.Find( name ); }
		void DestroyRendererWithName( std::wstring name ) { m_renderers.Destroy( name ); }
		void DestroyRenderer( Kiwi::Renderer* renderer ) { m_renderers.Destroy( renderer ); }

		Kiwi::RenderWindow* CreateRenderWindow( std::wstring windowName, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, bool vsyncEnabled );
		Kiwi::RenderWindow* CreateRenderWindow( std::wstring windowName, DWORD style, DWORD exStyle, const Kiwi::Vector2& position, 
												const Kiwi::Vector2& dimensions, bool vsyncEnabled, Kiwi::RenderWindow* parentWindow, HMENU menu );

		Kiwi::RenderWindow* FindRenderWindowWithName( std::wstring name ) { return m_renderWindows.Find( name ); }
		void DestroyRenderWindowWithName( std::wstring name ) { m_renderWindows.Destroy( name ); }
		void DestroyRenderWindow( Kiwi::RenderWindow* window ) { m_renderWindows.Destroy( window ); }

	};
}

#endif