#include "BlendStateManager.h"
#include "Renderer.h"

#include "../Core/Exception.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	BlendStateManager::BlendStateManager( Kiwi::Renderer* renderer )
	{

		assert( renderer != 0 );

		m_renderer = renderer;

	}

	Kiwi::BlendState* BlendStateManager::CreateBlendState( std::wstring name, const D3DBlendDesc& blendDesc, const D3DRenderTargetBlendDesc& rtBlendDesc, const Kiwi::Vector4& blendFactor, unsigned int sampleMask )
	{

		if( m_renderer )
		{
			ID3D11BlendState* d3dState = 0;

			D3D11_BLEND_DESC d3dDesc;

			d3dDesc.AlphaToCoverageEnable = blendDesc.alphaToCoverageEnable;
			d3dDesc.IndependentBlendEnable = blendDesc.independentBlendEnable;
			d3dDesc.RenderTarget[0] = rtBlendDesc.desc;

			HRESULT hr = m_renderer->GetDevice()->CreateBlendState( &d3dDesc, &d3dState );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"BlendStateManager::CreateBlendState", L"["+name+L"] Call to CreateBlendState failed: " + Kiwi::GetD3DErrorString( hr ) );
			}

			Kiwi::BlendState* blendState = new Kiwi::BlendState( name, d3dState, blendFactor, sampleMask );

			m_blendStates.Add( name, blendState );

			return blendState;
		}

		return 0;

	}

	void BlendStateManager::AddBlendState( Kiwi::BlendState* blendState )
	{

		if( blendState )
		{
			m_blendStates.Add( blendState->GetName(), blendState );
		}

	}

}