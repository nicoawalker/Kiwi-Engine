#include "RasterState.h"
#include "DirectX.h"
#include "Renderer.h"

#include "..\Core\Utilities.h"
#include "..\Core\Exception.h"

namespace Kiwi
{

	RasterState::RasterState( std::wstring name, Kiwi::Renderer* renderer, const Kiwi::D3DRasterStateDesc& desc )
	{

		assert( renderer != 0 );

		m_name = name;
		m_rasterDesc = desc;

		HRESULT hr = renderer->GetDevice()->CreateRasterizerState( &m_rasterDesc.desc, &m_rasterState );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"RasterState", L"[" + m_name + L"] Call to CreateRasterizerState failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

	}

	RasterState::~RasterState()
	{

		SAFE_RELEASE( m_rasterState );

	}
	
};