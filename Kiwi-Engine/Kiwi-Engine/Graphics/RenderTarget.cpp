#include "RenderTarget.h"
#include "Viewport.h"
#include "Camera.h"
#include "Renderer.h"
#include "BlendStateManager.h"
#include "Mesh.h"
#include "Renderer.h"

#include "../Types.h"

#include "..\Core\Scene.h"
#include "../Core/Exception.h"
#include "..\Core\Utilities.h"
#include "../Core/Assert.h"
#include "..\Core\IAssetFactory.h"
#include "../Core/EngineRoot.h"
#include "..\Core\TextureFactory.h"

namespace Kiwi
{

	RenderTarget::RenderTarget( std::wstring name, Kiwi::Scene* scene, Kiwi::Renderer& renderer, const Kiwi::Vector2d& dimensions ) :
		Kiwi::Texture( name ),
		m_renderQueue( *this )
	{
		m_name = name;
		m_depthStencil = nullptr;
		m_priorityLevel = 0;
		m_blendState = nullptr;
		m_renderTargetView = nullptr;
		m_depthStencil = nullptr;
		m_renderTargetScene = scene;
		m_hasTransparency = true;
		m_isActive = true;
		m_isRenderTexture = true;
		m_autoRenderEnabled = true;

		m_dimensions = dimensions;

		ID3D11Device* device = renderer.GetDevice();
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		HRESULT hr;

		ZeroMemory( &textureDesc, sizeof( textureDesc ) );
		//create a new texture
		textureDesc.Width = (UINT)m_dimensions.x;
		textureDesc.Height = (UINT)m_dimensions.y;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;

		textureDesc.SampleDesc.Count = renderer.GetMultisampleDesc().count;
		textureDesc.SampleDesc.Quality = renderer.GetMultisampleDesc().quality;
		textureDesc.Format = renderer.GetDXGIFormat();
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		hr = device->CreateTexture2D( &textureDesc, NULL, &m_texture );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"RenderTarget", L"[" + m_name + L"] Failed to create render target texture" );
		}

		rtViewDesc.Format = textureDesc.Format;
		rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtViewDesc.Texture2D.MipSlice = 0;

		hr = device->CreateRenderTargetView( m_texture, &rtViewDesc, &m_renderTargetView );
		if( FAILED( hr ) )
		{
			SAFE_RELEASE( m_texture );

			throw Kiwi::Exception( L"RenderTarget", L"[" + m_name + L"] Failed to create render target view" );
		}

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView( m_texture, &shaderResourceViewDesc, &m_shaderResource );
		if( FAILED( hr ) )
		{
			SAFE_RELEASE( m_texture );
			SAFE_RELEASE( m_shaderResource );

			throw Kiwi::Exception( L"RenderTarget", L"[" + m_name + L"] Failed to create render target shader resource" );
		}

		//setup the default blend state for render targets
		/*
		Additive blending: (source * 1) + (dest * 1)
		Multiplicative blending: (source * 0) + (dest * source_color)
		Alpha blending: (source * source_alpha) + (dest * inv_src_alpha)
		*/
		m_blendDesc.desc.BlendEnable = true;
		m_blendDesc.desc.SrcBlend = D3D11_BLEND_SRC_ALPHA; //blending for the new top pixel = source color * source alpha
		m_blendDesc.desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //blending for existing pixel to the inverse of the source alpha
		m_blendDesc.desc.BlendOp = D3D11_BLEND_OP_ADD;
		// dest.rgb = src.rgb * src.a + dest.rgb * (1 - src.a) ==> standard alpha blended color

		m_blendDesc.desc.SrcBlendAlpha = D3D11_BLEND_ONE;
		m_blendDesc.desc.DestBlendAlpha = D3D11_BLEND_ONE;
		m_blendDesc.desc.BlendOpAlpha = D3D11_BLEND_OP_MAX;
		// dest.a = src.a * (1 - dest.a) + dest.a
		//       == src.a + dest.a - src.a * dest.a
		//       == 1 - (1 - src.a) * (1 - dest.a) ==> preserves alpha to target (optional)
		m_blendDesc.desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_blendDesc.alphaToCoverageEnable = false;
		m_blendDesc.independentBlendEnable = false;

		m_blendState = renderer.CreateBlendState( m_name + L"/DefaultBlendState", m_blendDesc );
		if( m_blendState != nullptr )
		{
			m_blendStateName = m_blendState->GetName();
		}

		m_clearColor.Set( 1.0, 0.0, 1.0, 1.0 );
		m_blendingFactor.Set( 1.0, 1.0, 1.0, 1.0 );
	}

	RenderTarget::RenderTarget( std::wstring name, Kiwi::Scene* scene, Kiwi::Renderer& renderer, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource, ID3D11RenderTargetView* renderTargetView ):
		Kiwi::Texture( name, L"", texture, shaderResource, true ),
		m_renderQueue( *this )
	{
		if( texture == 0 )
		{
			throw Kiwi::Exception( L"RenderTarget::RenderTarget", L"Failed to create RenderTarget '" + name + L"': invalid texture", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( renderTargetView == 0 )
		{
			throw Kiwi::Exception( L"RenderTarget::RenderTarget", L"Failed to create RenderTarget '" + name + L"': invalid RenderTargetView", KIWI_EXCEPTION::NULLOPERATION );
		}

		m_name = name;
		m_renderTargetView = renderTargetView;
		m_depthStencil = nullptr;
		m_blendState = nullptr;
		m_renderTargetScene = scene;
		m_isActive = true;
		m_isRenderTexture = true;
		m_autoRenderEnabled = true;

		ID3D11Device* device = renderer.GetDevice();

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		texture->GetDesc( &textureDesc );

		m_dimensions = Kiwi::Vector2( (float)textureDesc.Width, (float)textureDesc.Height );

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		////setup the default blend state for render targets
		//m_blendDesc.desc.BlendEnable = true;
		//m_blendDesc.desc.SrcBlend = D3D11_BLEND_SRC_ALPHA; //blending for the new top pixel = source color * source alpha
		//m_blendDesc.desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //blending for existing pixel = 1.0 - source alpha value
		//m_blendDesc.desc.BlendOp = D3D11_BLEND_OP_ADD;
		//// dest.rgb = src.rgb * src.a + dest.rgb * (1 - src.a) ==> standard alpha blended color

		//m_blendDesc.desc.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
		//m_blendDesc.desc.DestBlendAlpha = D3D11_BLEND_ONE;
		//m_blendDesc.desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//// dest.a = src.a * (1 - dest.a) + dest.a
		////       == src.a + dest.a - src.a * dest.a
		////       == 1 - (1 - src.a) * (1 - dest.a) ==> preserves alpha to target (optional)
		//m_blendDesc.desc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		////create the default blend state and set it to be active
		//Kiwi::D3DBlendDescProperties tDesc;
		//tDesc.alphaToCoverageEnable = false;
		////tDesc.independentBlendEnable = false;
		//m_activeBlendState = this->CreateBlendState( L"default", tDesc, Kiwi::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
		////m_activeBlendState = this->CreateBlendState( L"default", tDesc, Kiwi::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//setup the default blend state for render targets
		m_blendDesc.desc.BlendEnable = true;
		m_blendDesc.desc.SrcBlend = D3D11_BLEND_SRC_ALPHA; //blending for the new top pixel = source color * source alpha
		m_blendDesc.desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //blending for existing pixel, set to use the blend factor to determine blending for each channel
		m_blendDesc.desc.BlendOp = D3D11_BLEND_OP_ADD;
		// dest.rgb = src.rgb * src.a + dest.rgb * (1 - src.a) ==> standard alpha blended color

		m_blendDesc.desc.SrcBlendAlpha = D3D11_BLEND_ONE; //use full source alpha
		m_blendDesc.desc.DestBlendAlpha = D3D11_BLEND_ONE; //use no destination alpha
		m_blendDesc.desc.BlendOpAlpha = D3D11_BLEND_OP_MAX;
		// dest.a = src.a * (1 - dest.a) + dest.a
		//       == src.a + dest.a - src.a * dest.a
		//       == 1 - (1 - src.a) * (1 - dest.a) ==> preserves alpha to target (optional)
		m_blendDesc.desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_blendDesc.alphaToCoverageEnable = false;
		m_blendDesc.independentBlendEnable = false;

		m_blendState = renderer.CreateBlendState( m_name + L"/DefaultBlendState", m_blendDesc );
		if( m_blendState != nullptr )
		{
			m_blendStateName = m_blendState->GetName();
		}

		m_clearColor.Set( 1.0, 0.0, 1.0, 1.0 );
		m_blendingFactor.Set( 1.0, 1.0, 1.0, 1.0 );
	}

	RenderTarget::~RenderTarget()
	{
		Kiwi::FreeMemory( m_viewports );

		/*release the d3d resources, unless this is the back buffer in which case they are released automatically in the D3D11Interface*/
		if( m_name.compare( L"Backbuffer" ) != 0 )
		{
			SAFE_RELEASE( m_renderTargetView );
		}
	}

	void RenderTarget::AttachDepthStencil(Kiwi::DepthStencil* depthStencil)
	{
		m_depthStencil = depthStencil;
	}

	void RenderTarget::CreateRenderGroup( const std::wstring& groupName, unsigned short viewport )
	{
		m_renderQueue.CreateRenderGroup( groupName, viewport );
	}

	Kiwi::Viewport* RenderTarget::CreateViewport( std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2d& position, const Kiwi::Vector2d& dimensions, float minDepth, float maxDepth )
	{
		/*the function is provided with dimensions and position relative to the size of
		the render target, so convert those into absolutes for the viewport*/
		Kiwi::Vector2d absoluteDimensions( m_dimensions.x * dimensions.x, m_dimensions.y * dimensions.y );
		Kiwi::Vector2d absolutePosition( m_dimensions.x * position.x, m_dimensions.y * position.y );

		m_viewports.push_back( Kiwi::Viewport( *this, name, camera, absolutePosition, absoluteDimensions, minDepth, maxDepth ) );

		return &m_viewports[m_viewports.size() - 1];
	}

	ID3D11RenderTargetView* RenderTarget::GetRenderTargetView()const
	{
		return m_renderTargetView;
	}

	Kiwi::Viewport* RenderTarget::GetViewport( unsigned int index )
	{
		if( index >= m_viewports.size() )
		{
			return 0;
		}

		return &m_viewports[index];
	}

	void RenderTarget::SetBlendingFactor( const Kiwi::Vector4d& blendFactor )
	{
		m_blendingFactor = blendFactor;
	}

	void RenderTarget::SetBlendState( const std::wstring& name )
	{
		if( name.compare( m_blendStateName ) != 0 )
		{
			m_blendState = nullptr;
			m_blendStateName = name;
		}
	}

	void RenderTarget::SetBlendState( Kiwi::BlendState* blendState )
	{
		if( m_blendState != blendState )
		{
			m_blendState = blendState;

			if( m_blendState == nullptr )
			{
				m_blendStateName = L"";

			} else
			{
				m_blendStateName = m_blendState->GetName();
			}
		}
	}

	void RenderTarget::SetClearColor( const Kiwi::Color& clearColor )
	{
		m_clearColor = clearColor;
	}

	void RenderTarget::OnPreRender()
	{
		/*build the render queue, then call each model's pre render function to prepare them for rendering*/
		m_renderQueue.Build();

		m_renderQueue.OnPreRender();
	}

	void RenderTarget::OnPostRender()
	{
		m_renderQueue.OnPostRender();

		m_renderQueue.Reset();
	}

};