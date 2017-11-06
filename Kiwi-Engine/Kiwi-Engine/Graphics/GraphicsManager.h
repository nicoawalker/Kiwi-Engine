#ifndef _KIWI_GRAPHICSMANAGER_H_
#define _KIWI_GRAPHICSMANAGER_H_

#include "Renderer.h"
#include "RenderWindow.h"

#include <unordered_map>
#include <string>
#include <mutex>

namespace Kiwi
{

	class EngineRoot;

	/*this class is responsible for managing all of the graphical assets, resources
	and classes used by the engine*/
	class GraphicsManager
	{
	friend class EngineRoot;

	protected:

		std::unordered_map<std::wstring, std::unique_ptr<Kiwi::Renderer>> m_renderers;
		std::unordered_map<std::wstring, std::unique_ptr<Kiwi::RenderWindow>> m_renderWindows;

		std::mutex m_graphicsManager_M;

		Kiwi::Renderer* m_activeRenderer;

	protected:

		void _DestroyRenderer( std::wstring name );
		void _DestroyRenderWindow( std::wstring name );
		Kiwi::RenderWindow* _FindRenderWindow( const std::wstring name );
		Kiwi::Renderer* _FindRenderer( const std::wstring name );

	public:

		GraphicsManager();
		~GraphicsManager();

		void Update();

		/*creates a new renderer. will either return a pointer to the new renderer or throw a Kiwi::Exception if creation failed
		>name: unique name of the renderer
		>renderWindowName: name of the render window to bind the renderer to
		>setDefault: if true, the new renderer will be set as the default renderer*/
		Kiwi::Renderer* CreateRenderer( std::wstring name, std::wstring renderWindowName );

		/*finds and returns the renderer with matching name
		returns 0 if no match is found*/
		Kiwi::Renderer* FindRenderer( std::wstring name );

		void DestroyRenderer( std::wstring name );
		void DestroyRenderer( Kiwi::Renderer* renderer );

		void SetActiveRenderer( const std::wstring& name );

		Kiwi::RenderWindow* CreateRenderWindow( std::wstring windowName, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, bool vsyncEnabled );
		Kiwi::RenderWindow* CreateRenderWindow( std::wstring windowName, DWORD style, DWORD exStyle, const Kiwi::Vector2& position, 
												const Kiwi::Vector2& dimensions, bool vsyncEnabled, Kiwi::RenderWindow* parentWindow, HMENU menu );

		/*finds and returns the renderwindow with matching name
		returns 0 if no match is found*/
		Kiwi::RenderWindow* FindRenderWindow( std::wstring name );

		void DestroyRenderWindow( std::wstring name );
		void DestroyRenderWindow( Kiwi::RenderWindow* window );

		Kiwi::Renderer* GetActiveRenderer()const { return m_activeRenderer; }

		void Shutdown();
	};
}

#endif