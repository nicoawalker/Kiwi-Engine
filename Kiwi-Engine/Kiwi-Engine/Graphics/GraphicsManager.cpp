#include "GraphicsManager.h"

#include "../Core/Exception.h"
#include "..\Core\EngineRoot.h"

namespace Kiwi
{

	GraphicsManager::GraphicsManager()
	{
		m_activeRenderer = nullptr;
	}

	GraphicsManager::~GraphicsManager()
	{
	}

	void GraphicsManager::_DestroyRenderWindow( std::wstring name )
	{
		auto renderWindowItr = m_renderWindows.find( name );
		if( renderWindowItr != m_renderWindows.end() )
		{
			m_renderWindows.erase( renderWindowItr );
		}
	}

	void GraphicsManager::_DestroyRenderer( std::wstring name )
	{
		auto rendererItr = m_renderers.find( name );
		if( rendererItr != m_renderers.end() )
		{
			if( m_activeRenderer == rendererItr->second.get() )
			{
				m_activeRenderer = nullptr;
			}

			m_renderers.erase( rendererItr );
		}
	}

	Kiwi::RenderWindow* GraphicsManager::_FindRenderWindow( const std::wstring name )
	{
		auto renderWindowItr = m_renderWindows.find( name );
		if( renderWindowItr != m_renderWindows.end() )
		{
			return renderWindowItr->second.get();
		}

		return 0;
	}

	Kiwi::Renderer* GraphicsManager::_FindRenderer( const std::wstring name )
	{
		auto rendererItr = m_renderers.find( name );
		if( rendererItr != m_renderers.end() )
		{
			return rendererItr->second.get();
		}

		return 0;
	}

	void GraphicsManager::Update()
	{
		for( auto itr = m_renderWindows.begin(); itr != m_renderWindows.end(); itr++ )
		{
			if( itr->second )
			{
				itr->second->Update();
			}
		}

		for( auto& keyValuePair : m_renderers )
		{
			if(keyValuePair.second != nullptr) keyValuePair.second->Update();
		}
	}

	Kiwi::Renderer* GraphicsManager::CreateRenderer( std::wstring name, std::wstring renderWindowName )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		bool setActive = false;

		if( this->_FindRenderer( name ) != 0 )
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderer", L"Failed to create renderer '" + name + L"': duplicate renderer name", KIWI_EXCEPTION::DUPLICATIONATTEMPT );
		}

		Kiwi::RenderWindow* renderWindow = this->_FindRenderWindow( renderWindowName );
		if( renderWindow == 0 )
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderer", L"Failed to create renderer '" + name + L"': render window '" + renderWindowName + L"' is invalid", KIWI_EXCEPTION::DUPLICATIONATTEMPT );
		}

		std::unique_ptr<Kiwi::Renderer> renderer = std::make_unique<Kiwi::Renderer>( name, *renderWindow );
		if( renderer == nullptr )
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderer", L"Failed to create renderer '" + name + L"': allocation failed", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		/*if no renderers exist yet set this first one as the active one*/
		if( m_renderers.size() == 0 ) setActive = true;

		auto rItr = m_renderers.insert( std::make_pair( name, std::move( renderer ) ) );

		/*check whether the renderer was successfully inserted and return a pointer to the renderer if it was*/
		if( rItr.second )
		{
			if( setActive ) m_activeRenderer = rItr.first->second.get();

			return rItr.first->second.get();

		} else
		{
			return nullptr;
		}
	}

	Kiwi::RenderWindow* GraphicsManager::CreateRenderWindow( std::wstring windowName, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, bool vsyncEnabled )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		if( this->_FindRenderWindow( windowName ) != 0 )
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderWindow", L"Failed to create RenderWindow '" + windowName + L"': duplicate window name", KIWI_EXCEPTION::DUPLICATIONATTEMPT );
		}

		Kiwi::RenderWindow* renderWindow = new Kiwi::RenderWindow( windowName, WS_OVERLAPPEDWINDOW, 0, (int)position.x, (int)position.y, (int)dimensions.x, (int)dimensions.y, vsyncEnabled, NULL, NULL );
		if( renderWindow == 0 )
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderWindow", L"Failed to create RenderWindow '" + windowName + L"': allocation failed", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		m_renderWindows.insert( std::make_pair( windowName, std::unique_ptr<Kiwi::RenderWindow>( renderWindow ) ) );

		return renderWindow;
	}

	Kiwi::RenderWindow* GraphicsManager::CreateRenderWindow( std::wstring windowName, DWORD style, DWORD exStyle, const Kiwi::Vector2& position,
															 const Kiwi::Vector2& dimensions, bool vsyncEnabled, Kiwi::RenderWindow* parentWindow, HMENU menu )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		if( this->_FindRenderWindow( windowName ) != 0 )
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderWindow", L"Failed to create RenderWindow '" + windowName + L"': duplicate window name", KIWI_EXCEPTION::DUPLICATIONATTEMPT );
		}

		HWND parentHWND = 0;
		if( parentWindow )
		{
			parentHWND = parentWindow->GetHandle();
		}

		Kiwi::RenderWindow* renderWindow = new Kiwi::RenderWindow( windowName, style, exStyle, (int)position.x, (int)position.y, (int)dimensions.x, (int)dimensions.y, vsyncEnabled, parentHWND, menu );
		if( renderWindow == 0 )
		{
			throw Kiwi::Exception( L"GraphicsManager::CreateRenderWindow", L"Failed to create RenderWindow '" + windowName + L"': allocation failed", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		m_renderWindows.insert( std::make_pair( windowName, std::unique_ptr<Kiwi::RenderWindow>( renderWindow ) ) );

		return renderWindow;
	}

	Kiwi::Renderer* GraphicsManager::FindRenderer( std::wstring name )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		return this->_FindRenderer( name );
	}

	void GraphicsManager::DestroyRenderer( std::wstring name )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		this->_DestroyRenderer( name );
	}

	void GraphicsManager::DestroyRenderer( Kiwi::Renderer* renderer )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		if( renderer != 0 )
		{
			this->_DestroyRenderer( renderer->GetName() );
		}
	}

	void GraphicsManager::SetActiveRenderer( const std::wstring& name )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		Kiwi::Renderer* renderer = _FindRenderer( name );
		if( renderer != nullptr )
		{
			m_activeRenderer = renderer;
		}
	}

	Kiwi::RenderWindow* GraphicsManager::FindRenderWindow( std::wstring name )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		return this->_FindRenderWindow( name );
	}

	void GraphicsManager::DestroyRenderWindow( std::wstring name )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		this->_DestroyRenderWindow( name );
	}

	void GraphicsManager::DestroyRenderWindow( Kiwi::RenderWindow* window )
	{
		std::lock_guard<std::mutex> lock( m_graphicsManager_M );

		if( window != 0 )
		{
			this->_DestroyRenderWindow( window->GetName() );
		}
	}

	void GraphicsManager::Shutdown()
	{
		for( auto itr = m_renderWindows.begin(); itr != m_renderWindows.end(); itr++ )
		{
			itr->second->Shutdown();
		}

		for( auto itr = m_renderers.begin(); itr != m_renderers.end(); itr++ )
		{
			itr->second->Shutdown();
		}
	}

}