#include "RasterStateManager.h"
#include "DirectX.h"
#include "Renderer.h"
#include "RasterState.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"

namespace Kiwi
{

	RasterStateManager::RasterStateManager( Kiwi::Renderer& renderer )
	{
		
		m_renderer = &renderer;

	}

	RasterStateManager::~RasterStateManager()
	{
	}

	Kiwi::RasterState* RasterStateManager::CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc )
	{

		Kiwi::RasterState* rasterState = new Kiwi::RasterState( name, m_renderer, desc );

		m_rasterStates.Add( name, rasterState );

		return rasterState;

	}

	void RasterStateManager::AddRasterState( Kiwi::RasterState* rasterState )
	{

		if( rasterState )
		{
			m_rasterStates.Add( rasterState->GetName(), rasterState );
		}

	}

	void RasterStateManager::DestroyAllRasterStates()
	{

		m_rasterStates.DestroyAll();

	}

	void RasterStateManager::DestroyRasterStateWithName(std::wstring name)
	{

		m_rasterStates.Destroy( name );

	}

	Kiwi::RasterState* RasterStateManager::FindRasterStateWithName(std::wstring name)
	{

		return m_rasterStates.Find( name );

	}

};