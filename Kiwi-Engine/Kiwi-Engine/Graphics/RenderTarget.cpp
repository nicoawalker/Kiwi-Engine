#include "RenderTarget.h"
#include "Viewport.h"
#include "Camera.h"
#include "Renderer.h"
#include "BlendStateManager.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	RenderTarget::RenderTarget( std::wstring name, Kiwi::Renderer* renderer, unsigned int width, unsigned int height )
	{

		assert( renderer != 0 );

		m_name = name;
		m_depthStencil = 0;
		m_priorityLevel = 0;
		m_activeBlendState = 0;

		m_dimensions = Kiwi::Vector2( (float)width, (float)height );

		ID3D11Device* device = renderer->GetDevice();
		ID3D11ShaderResourceView* shaderRes = 0;

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		HRESULT hr;

		ZeroMemory( &textureDesc, sizeof( textureDesc ) );
		//create a new texture
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;

		textureDesc.SampleDesc.Count = renderer->GetMultisampleDesc().count;
		textureDesc.SampleDesc.Quality = renderer->GetMultisampleDesc().quality;
		textureDesc.Format = renderer->GetDXGIFormat();
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		hr = device->CreateTexture2D( &textureDesc, NULL, &m_renderTexture );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"RenderTarget", L"["+m_name+L"] Failed to create render target texture" );
		}

		rtViewDesc.Format = textureDesc.Format;
		rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtViewDesc.Texture2D.MipSlice = 0;

		hr = device->CreateRenderTargetView( m_renderTexture, &rtViewDesc, &m_renderView );
		if( FAILED( hr ) )
		{
			SAFE_RELEASE( m_renderTexture );

			throw Kiwi::Exception( L"RenderTarget", L"[" + m_name + L"] Failed to create render target view" );
		}

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView( m_renderTexture, &shaderResourceViewDesc, &m_shaderResource );
		if( FAILED( hr ) )
		{
			SAFE_RELEASE( m_renderTexture );
			SAFE_RELEASE( m_renderView );

			throw Kiwi::Exception( L"RenderTarget", L"[" + m_name + L"] Failed to create render target shader resource" );
		}

		//create the blend state manager to create and hold this render target's blend states
		m_blendStateManager = new Kiwi::BlendStateManager( renderer );
		//setup the default blend state for render targets
		m_blendDesc.desc.BlendEnable = true;
		m_blendDesc.desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		m_blendDesc.desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		m_blendDesc.desc.BlendOp = D3D11_BLEND_OP_ADD;
		m_blendDesc.desc.SrcBlendAlpha = D3D11_BLEND_ONE;
		m_blendDesc.desc.DestBlendAlpha = D3D11_BLEND_ZERO;
		m_blendDesc.desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		m_blendDesc.desc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		//create the default blend state and set it to be active
		Kiwi::D3DBlendDesc tDesc;
		tDesc.alphaToCoverageEnable = false;
		m_activeBlendState = this->CreateBlendState( L"default", tDesc, Kiwi::Vector4( 0.75f, 0.75f, 0.75f, 1.0f ) );

	}

	RenderTarget::RenderTarget( std::wstring name, Kiwi::Renderer* renderer, ID3D11Texture2D* texture, ID3D11RenderTargetView* renderTargetView )
	{

		assert( renderer != 0 && texture != 0 && renderTargetView != 0 );

		m_name = name;
		m_renderTexture = texture;
		m_renderView = renderTargetView;
		m_shaderResource = 0;
		m_depthStencil = 0;
		m_activeBlendState = 0;

		ID3D11Device* device = renderer->GetDevice();

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		m_renderTexture->GetDesc( &textureDesc );

		m_dimensions = Kiwi::Vector2( (float)textureDesc.Width, (float)textureDesc.Height );

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		/*HRESULT hr = device->CreateShaderResourceView( m_renderTexture, &shaderResourceViewDesc, &m_shaderResource );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::CreateRenderTarget", L"["+m_name+L"] Failed to create shader resource view" );
		}*/

		//create the blend state manager to create and hold this render target's blend states
		m_blendStateManager = new Kiwi::BlendStateManager( renderer );

		//setup the default blend state for render targets
		m_blendDesc.desc.BlendEnable = true;
		m_blendDesc.desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		m_blendDesc.desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		m_blendDesc.desc.BlendOp = D3D11_BLEND_OP_ADD;
		m_blendDesc.desc.SrcBlendAlpha = D3D11_BLEND_ONE;
		m_blendDesc.desc.DestBlendAlpha = D3D11_BLEND_ZERO;
		m_blendDesc.desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		m_blendDesc.desc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		//create the default blend state and set it to be active
		Kiwi::D3DBlendDesc tDesc;
		tDesc.alphaToCoverageEnable = false;
		m_activeBlendState = this->CreateBlendState( L"default", tDesc, Kiwi::Vector4( 0.75f, 0.75f, 0.75f, 1.0f ) );

	}

	RenderTarget::~RenderTarget()
	{

		m_depthStencil = 0;

		SAFE_DELETE( m_blendStateManager );
		SAFE_RELEASE(m_renderTexture);
		SAFE_RELEASE(m_renderView);
		SAFE_RELEASE(m_shaderResource);

	}

	void RenderTarget::AttachDepthStencil(Kiwi::DepthStencil* depthStencil)
	{

		m_depthStencil = depthStencil;

	}

	void RenderTarget::SetBlendState( std::wstring blendStateName )
	{

		Kiwi::BlendState* blendState = this->FindBlendStateWithName( blendStateName );

		if( blendState != 0 ) m_activeBlendState = blendState;

	}

	Kiwi::Viewport* RenderTarget::CreateViewport(std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, float minDepth, float maxDepth)
	{

		/*the function is provided with dimensions and position relative to the size of
		the render target, so convert those into absolutes for the viewport*/
		Kiwi::Vector2 absoluteDimensions( m_dimensions.x * dimensions.x, m_dimensions.y * dimensions.y );
		Kiwi::Vector2 absolutePosition( m_dimensions.x * position.x, m_dimensions.y * position.y );

		Kiwi::Viewport newViewport( name, camera, absolutePosition, absoluteDimensions, minDepth, maxDepth);

		m_viewports.push_back(newViewport);

		return &m_viewports[m_viewports.size() - 1];

	}

	Kiwi::BlendState* RenderTarget::CreateBlendState( std::wstring name, const D3DBlendDesc& blendDesc, const Kiwi::Vector4& blendFactor, unsigned int sampleMask )
	{

		if( m_blendStateManager )
		{
			return m_blendStateManager->CreateBlendState( name, blendDesc, m_blendDesc, blendFactor, sampleMask );
		}

		return 0;

	}

	Kiwi::BlendState* RenderTarget::FindBlendStateWithName( std::wstring name )
	{

		if( m_blendStateManager )
		{
			return m_blendStateManager->FindBlendStateWithName( name );
		}

		return 0;

	}

	Kiwi::Viewport* RenderTarget::GetViewport(unsigned int index)
	{

		if(index >= m_viewports.size())
		{
			return 0;
		}

		return &m_viewports[index];

	}

};