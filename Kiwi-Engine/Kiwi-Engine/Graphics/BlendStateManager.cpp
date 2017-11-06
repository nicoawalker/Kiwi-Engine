#include "BlendStateManager.h"
#include "Renderer.h"

#include "../Core/Exception.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	BlendStateManager::BlendStateManager( Kiwi::Renderer& renderer )
	{

		m_renderer = &renderer;

	}

	Kiwi::BlendState* BlendStateManager::_FindBlendState( const ObjectMultiKey& key )
	{

		auto itr = m_blendStates.find( key );
		if( itr != m_blendStates.end() )
		{
			return itr->second.get();
		}

		return 0;

	}

	void BlendStateManager::_DestroyBlendState( const ObjectMultiKey& key )
	{

		auto itr = m_blendStates.find( key );
		if( itr != m_blendStates.end() )
		{
			m_blendStates.erase( itr );
		}

	}

	Kiwi::BlendState* BlendStateManager::CreateBlendState( std::wstring name, const D3DBlendDescProperties& blendDesc, const D3DRenderTargetBlendDesc& rtBlendDesc, const Kiwi::Vector4& blendFactor, unsigned int sampleMask )
	{

		ID3D11BlendState* d3dState = 0;

		D3D11_BLEND_DESC d3dDesc;

		d3dDesc.AlphaToCoverageEnable = blendDesc.alphaToCoverageEnable;
		d3dDesc.IndependentBlendEnable = blendDesc.independentBlendEnable;
		d3dDesc.RenderTarget[0] = rtBlendDesc.desc;

		HRESULT hr = m_renderer->GetDevice()->CreateBlendState( &d3dDesc, &d3dState );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"BlendStateManager::CreateBlendState", L"[" + name + L"] Call to CreateBlendState failed: " + Kiwi::GetD3DErrorString( hr ), KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		Kiwi::BlendState* blendState = new Kiwi::BlendState( name, d3dState, blendFactor, sampleMask );

		m_blendStates.insert( std::make_pair( ObjectMultiKey( name, blendState->GetUID() ), std::unique_ptr<Kiwi::BlendState>( blendState ) ) );

		return blendState;

	}

	void BlendStateManager::AddBlendState( std::unique_ptr<Kiwi::BlendState> blendState )
	{

		if( blendState != 0 )
		{
			m_blendStates.insert( std::make_pair( ObjectMultiKey( blendState->GetName(), blendState->GetUID() ), std::move( blendState ) ) );
		}

	}

	Kiwi::BlendState* BlendStateManager::FindBlendState( std::wstring name )
	{

		return this->_FindBlendState( ObjectMultiKey( name, 0 ) );

	}

	Kiwi::BlendState* BlendStateManager::FindBlendState( long uid )
	{

		return this->_FindBlendState( ObjectMultiKey( L"", uid ) );

	}

	void BlendStateManager::DestroyBlendState( std::wstring name )
	{

		this->_DestroyBlendState( ObjectMultiKey( name, 0 ) );

	}

	void BlendStateManager::DestroyBlendState( long uid )
	{

		this->_DestroyBlendState( ObjectMultiKey( L"", uid ) );

	}

}