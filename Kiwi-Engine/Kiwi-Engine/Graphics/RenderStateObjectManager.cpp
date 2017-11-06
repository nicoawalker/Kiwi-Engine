#include "RenderStateObjectManager.h"
#include "Renderer.h"

namespace Kiwi
{

	RenderStateObjectManager::RenderStateObjectManager( Kiwi::Renderer& renderer )
	{
		m_renderer = &renderer;
	}

	RenderStateObjectManager::~RenderStateObjectManager()
	{
		Kiwi::FreeMemory( m_blendStates );
		Kiwi::FreeMemory( m_depthStencils );
		Kiwi::FreeMemory( m_rasterStates );
	}

	Kiwi::BlendState* RenderStateObjectManager::_FindBlendState( const ObjectMultiKey& key )
	{
		auto itr = m_blendStates.find( key );
		if( itr != m_blendStates.end() )
		{
			return itr->second.get();
		}

		return 0;
	}

	Kiwi::DepthStencil* RenderStateObjectManager::_FindDepthStencil( const ObjectMultiKey& key )
	{
		auto itr = m_depthStencils.find( key );
		if( itr != m_depthStencils.end() )
		{
			return itr->second.get();
		}

		return 0;
	}

	Kiwi::RasterState* RenderStateObjectManager::_FindRasterState( const ObjectMultiKey& key )
	{
		auto itr = m_rasterStates.find( key );
		if( itr != m_rasterStates.end() )
		{
			return itr->second.get();
		}

		return 0;
	}

	void RenderStateObjectManager::_DestroyBlendState( const ObjectMultiKey& key )
	{
		auto itr = m_blendStates.find( key );
		if( itr != m_blendStates.end() )
		{
			m_blendStates.erase( itr );
		}
	}

	void RenderStateObjectManager::_DestroyDepthStencil( const ObjectMultiKey& key )
	{
		auto itr = m_depthStencils.find( key );
		if( itr != m_depthStencils.end() )
		{
			m_depthStencils.erase( itr );
		}
	}

	void RenderStateObjectManager::_DestroyRasterState( const ObjectMultiKey& key )
	{
		auto itr = m_rasterStates.find( key );
		if( itr != m_rasterStates.end() )
		{
			m_rasterStates.erase( itr );
		}
	}

	Kiwi::RasterState* RenderStateObjectManager::CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc )
	{
		Kiwi::RasterState* rasterState = new Kiwi::RasterState( name, m_renderer, desc );

		m_rasterStates.insert( std::make_pair( ObjectMultiKey( name, rasterState->GetUID() ), std::unique_ptr<Kiwi::RasterState>( rasterState ) ) );

		return rasterState;
	}

	Kiwi::BlendState* RenderStateObjectManager::CreateBlendState( std::wstring name, const KiwiBlendDesc& rtBlendDesc, unsigned int sampleMask )
	{
		ID3D11BlendState* d3dState = 0;

		D3D11_BLEND_DESC d3dDesc;

		d3dDesc.AlphaToCoverageEnable = rtBlendDesc.alphaToCoverageEnable;
		d3dDesc.IndependentBlendEnable = rtBlendDesc.independentBlendEnable;
		d3dDesc.RenderTarget[0] = rtBlendDesc.desc;

		HRESULT hr = m_renderer->GetDevice()->CreateBlendState( &d3dDesc, &d3dState );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"BlendStateManager::CreateBlendState", L"[" + name + L"] Call to CreateBlendState failed: " + Kiwi::GetD3DErrorString( hr ), KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		Kiwi::BlendState* blendState = new Kiwi::BlendState( name, d3dState, sampleMask );

		m_blendStates.insert( std::make_pair( ObjectMultiKey( name, blendState->GetUID() ), std::unique_ptr<Kiwi::BlendState>( blendState ) ) );

		return blendState;
	}

	Kiwi::DepthStencil* RenderStateObjectManager::CreateDepthStencil( std::wstring name, const DepthBufferProperties& bufferProperties, const DepthStencilProperties& stencilProperties )
	{
		ID3D11DepthStencilState* depthDisabledState;
		ID3D11DepthStencilState* depthState;
		ID3D11DepthStencilView* depthView;
		ID3D11Texture2D* depthBuffer;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_DESC depthDisabledDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;

		//create depth buffer texture used for proper rendering in 3d space and allow for effects
		ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );
		if( bufferProperties.width == -1 )
		{
			depthBufferDesc.Width = (unsigned int)m_renderer->GetBackBuffer()->GetDimensions().x;
			depthBufferDesc.Height = (unsigned int)m_renderer->GetBackBuffer()->GetDimensions().y;
		} else
		{
			depthBufferDesc.Width = (unsigned int)bufferProperties.width;
			depthBufferDesc.Height = (unsigned int)bufferProperties.height;
		}
		depthBufferDesc.MipLevels = bufferProperties.mipLevels;
		depthBufferDesc.ArraySize = bufferProperties.arraySize;
		depthBufferDesc.Format = bufferProperties.format;
		depthBufferDesc.SampleDesc.Count = m_renderer->GetMultisampleDesc().count;
		depthBufferDesc.SampleDesc.Quality = m_renderer->GetMultisampleDesc().quality;
		depthBufferDesc.Usage = bufferProperties.usage;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = bufferProperties.cpuAccessFlags;
		depthBufferDesc.MiscFlags = bufferProperties.miscFlags;

		HRESULT hr = m_renderer->GetDevice()->CreateTexture2D( &depthBufferDesc, NULL, &depthBuffer );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"RenderStateObjectManager::CreateDepthStencil", L"[" + name + L"] Failed to create depth buffer" );
		}

		//now setup depth stencil to allow control of what type of depth test is done for each pixel
		ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = stencilProperties.depthWriteMask;
		depthStencilDesc.DepthFunc = stencilProperties.depthFunction;
		depthStencilDesc.StencilEnable = stencilProperties.enableStencil;
		depthStencilDesc.StencilReadMask = stencilProperties.stencilReadMask;
		depthStencilDesc.StencilWriteMask = stencilProperties.stencilWriteMask;
		//for front-facing pixels
		depthStencilDesc.FrontFace.StencilFailOp = stencilProperties.frontFailOp;
		depthStencilDesc.FrontFace.StencilDepthFailOp = stencilProperties.frontDepthFailOp;
		depthStencilDesc.FrontFace.StencilPassOp = stencilProperties.frontPassOp;
		depthStencilDesc.FrontFace.StencilFunc = stencilProperties.frontStencilFunc;
		//for back facing pixels
		depthStencilDesc.BackFace.StencilFailOp = stencilProperties.backFailOp;
		depthStencilDesc.BackFace.StencilDepthFailOp = stencilProperties.backDepthFailOp;
		depthStencilDesc.BackFace.StencilPassOp = stencilProperties.backPassOp;
		depthStencilDesc.BackFace.StencilFunc = stencilProperties.backStencilFunc;

		hr = m_renderer->GetDevice()->CreateDepthStencilState( &depthStencilDesc, &depthState );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"RenderStateObjectManager::CreateDepthStencil", L"[" + name + L"] Failed to create depth stencil state" );
		}

		//also create the depth-disabled state if needed
		ZeroMemory( &depthDisabledDesc, sizeof( depthDisabledDesc ) );
		depthDisabledDesc.DepthEnable = false;
		depthDisabledDesc.DepthWriteMask = stencilProperties.depthWriteMask;
		depthDisabledDesc.DepthFunc = stencilProperties.depthFunction;
		depthDisabledDesc.StencilEnable = stencilProperties.enableStencil;
		depthDisabledDesc.StencilReadMask = stencilProperties.stencilReadMask;
		depthDisabledDesc.StencilWriteMask = stencilProperties.stencilWriteMask;
		//for front-facing pixels
		depthDisabledDesc.FrontFace.StencilFailOp = stencilProperties.frontFailOp;
		depthDisabledDesc.FrontFace.StencilDepthFailOp = stencilProperties.frontDepthFailOp;
		depthDisabledDesc.FrontFace.StencilPassOp = stencilProperties.frontPassOp;
		depthDisabledDesc.FrontFace.StencilFunc = stencilProperties.frontStencilFunc;
		//for back facing pixels
		depthDisabledDesc.BackFace.StencilFailOp = stencilProperties.backFailOp;
		depthDisabledDesc.BackFace.StencilDepthFailOp = stencilProperties.backDepthFailOp;
		depthDisabledDesc.BackFace.StencilPassOp = stencilProperties.backPassOp;
		depthDisabledDesc.BackFace.StencilFunc = stencilProperties.backStencilFunc;

		hr = m_renderer->GetDevice()->CreateDepthStencilState( &depthDisabledDesc, &depthDisabledState );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"RenderStateObjectManager::CreateDepthStencil", L"[" + name + L"] Failed to create depth disabled stencil state" );
		}

		//setup the depth stencil buffer so DX will use the buffer as a depth stencil texture
		ZeroMemory( &dsViewDesc, sizeof( dsViewDesc ) );
		dsViewDesc.Format = depthBufferDesc.Format;
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsViewDesc.Texture2D.MipSlice = 0;

		hr = m_renderer->GetDevice()->CreateDepthStencilView( depthBuffer, &dsViewDesc, &depthView );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"RenderStateObjectManager::CreateDepthStencil", L"[" + name + L"] Failed to create depth stencil view" );
		}

		Kiwi::DepthStencil* stencil = new Kiwi::DepthStencil( name, depthBuffer, depthView, depthState, depthDisabledState );

		m_depthStencils.insert( std::make_pair( ObjectMultiKey( name, stencil->GetUID() ), std::unique_ptr<Kiwi::DepthStencil>( stencil ) ) );

		return stencil;
	}

	Kiwi::DepthStencil* RenderStateObjectManager::CreateDepthStencil( std::wstring name, ID3D11Texture2D* depthBuffer, ID3D11DepthStencilView* depthView, ID3D11DepthStencilState* depthState, ID3D11DepthStencilState* depthDisabledState )
	{
		Kiwi::DepthStencil* stencil = new Kiwi::DepthStencil( name, depthBuffer, depthView, depthState, depthDisabledState );

		m_depthStencils.insert( std::make_pair( ObjectMultiKey( name, stencil->GetUID() ), std::unique_ptr<Kiwi::DepthStencil>( stencil ) ) );

		return stencil;
	}

	Kiwi::BlendState* RenderStateObjectManager::FindBlendState( const std::wstring& name )
	{
		return this->_FindBlendState( ObjectMultiKey( name, 0 ) );
	}

	Kiwi::RasterState* RenderStateObjectManager::FindRasterState( std::wstring name )
	{
		return this->_FindRasterState( ObjectMultiKey( name, 0 ) );
	}

	Kiwi::RasterState* RenderStateObjectManager::FindRasterState( long uid )
	{
		return this->_FindRasterState( ObjectMultiKey( L"", uid ) );
	}

	Kiwi::DepthStencil* RenderStateObjectManager::FindDepthStencil( std::wstring name )
	{
		return this->_FindDepthStencil( ObjectMultiKey( name, 0 ) );
	}

	Kiwi::DepthStencil* RenderStateObjectManager::FindDepthStencil( long uid )
	{
		return this->_FindDepthStencil( ObjectMultiKey( L"", uid ) );
	}

	void RenderStateObjectManager::DestroyBlendState( const std::wstring& name )
	{
		this->_DestroyBlendState( ObjectMultiKey( name, 0 ) );
	}

	void RenderStateObjectManager::DestroyRasterState( std::wstring name )
	{
		this->_DestroyRasterState( ObjectMultiKey( name, 0 ) );
	}

	void RenderStateObjectManager::DestroyRasterState( long uid )
	{
		this->_DestroyRasterState( ObjectMultiKey( L"", uid ) );
	}

	void RenderStateObjectManager::DestroyDepthStencil( std::wstring name )
	{
		this->_DestroyDepthStencil( ObjectMultiKey( name, 0 ) );
	}

	void RenderStateObjectManager::DestroyDepthStencil( long uid )
	{
		this->_DestroyDepthStencil( ObjectMultiKey( L"", uid ) );
	}

	void RenderStateObjectManager::Shutdown()
	{
		Kiwi::FreeMemory( m_blendStates );
		Kiwi::FreeMemory( m_depthStencils );
		Kiwi::FreeMemory( m_rasterStates );
	}

}