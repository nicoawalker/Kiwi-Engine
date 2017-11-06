#include "RenderTargetManager.h"
#include "Renderer.h"

#include "../Core/Utilities.h"
#include "..\Core\Exception.h"

#include <algorithm>

namespace Kiwi
{

	RenderTargetManager::RenderTargetManager( Kiwi::Renderer& renderer )
	{

		m_renderer = &renderer;

	}

	Kiwi::RenderTarget* RenderTargetManager::CreateRenderTarget( std::wstring name, Kiwi::Scene* scene, const Kiwi::Vector2d& dimensions )
	{

		if( this->FindRenderTarget( name ) != 0 )
		{
			throw Kiwi::Exception( L"RenderTargetManager::CreateRenderTarget", L"Failed to create RenderTarget '" + name + L"': duplicate name", KIWI_EXCEPTION::DUPLICATIONATTEMPT );
		}

		Kiwi::RenderTarget* newTarget = new Kiwi::RenderTarget( name, scene, *m_renderer, dimensions );
		if( newTarget == 0 )
		{
			throw Kiwi::Exception( L"RenderTargetManager::CreateRenderTarget", L"Failed to create RenderTarget '" + name + L"': allocation failed", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		m_renderTargets.insert( std::make_pair( name, std::unique_ptr<Kiwi::RenderTarget>( newTarget ) ) );

		return newTarget;

	}

	Kiwi::RenderTarget* RenderTargetManager::CreateRenderTarget( std::wstring name, Kiwi::Scene* scene, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource, ID3D11RenderTargetView* renderTargetView )
	{

		if( this->FindRenderTarget( name ) != 0 )
		{
			throw Kiwi::Exception( L"RenderTargetManager::CreateRenderTarget", L"Failed to create RenderTarget '" + name + L"': duplicate name", KIWI_EXCEPTION::DUPLICATIONATTEMPT );
		}

		Kiwi::RenderTarget* newTarget = new Kiwi::RenderTarget( name, scene, *m_renderer, texture, shaderResource, renderTargetView );
		if( newTarget == 0 )
		{
			throw Kiwi::Exception( L"RenderTargetManager::CreateRenderTarget", L"Failed to create RenderTarget '" + name + L"': allocation failed", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		m_renderTargets.insert( std::make_pair( name, std::unique_ptr<Kiwi::RenderTarget>( newTarget ) ) );

		return newTarget;

	}

	Kiwi::RenderTarget* RenderTargetManager::FindRenderTarget( std::wstring name )
	{

		auto targetItr = m_renderTargets.find( name );
		if( targetItr != m_renderTargets.end() )
		{
			if( targetItr->second != 0 )
			{
				return targetItr->second.get();

			} else
			{
				/*remove any null pointers somehow in the array*/
				m_renderTargets.erase( targetItr );
			}
		}

		return 0;

	}

	void RenderTargetManager::DestroyRenderTarget( std::wstring name )
	{

		auto targetItr = m_renderTargets.find( name );
		if( targetItr != m_renderTargets.end() )
		{
			m_renderTargets.erase( targetItr );
		}

	}

	Kiwi::RenderTargetList RenderTargetManager::GetRenderTargetList()
	{

		Kiwi::RenderTargetList list;

		for( auto itr = m_renderTargets.begin(); itr != m_renderTargets.end(); itr++ )
		{
			list.targets.push_back( itr->second.get() );
		}

		return list;

	}

	void RenderTargetManager::Shutdown()
	{
		Kiwi::FreeMemory( m_renderTargets );
	}

}