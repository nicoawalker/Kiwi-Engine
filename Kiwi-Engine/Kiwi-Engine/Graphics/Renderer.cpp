#include "Renderer.h"
#include "D3D11Interface.h"
#include "RenderWindow.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RasterStateManager.h"
#include "BlendState.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	Renderer::Renderer(std::wstring name, Kiwi::RenderWindow* window)
	{

		if(window == 0)
		{
			throw Kiwi::Exception(L"Renderer", L"Invalid window");
		}

		m_d3dInterface = 0;
		m_rasterStateManager = 0;
		m_backBuffer = 0;

		try
		{
			//create the D3D interface
			m_d3dInterface = new Kiwi::D3D11Interface(window->GetHandle(), window->IsFullscreen(), (unsigned int)window->GetClientSize().x, (unsigned int)window->GetClientSize().y, window->GetVSyncEnabled());
			
			//create a default rendertarget for the back buffer
			Kiwi::BackBuffer* bb = m_d3dInterface->GetBackBuffer();
			m_backBuffer = new Kiwi::RenderTarget( L"Back Buffer", this, bb->texture, bb->renderTargetView );

			//create the back buffer's depth stencil
			Kiwi::DepthStencil* bbDepth = new Kiwi::DepthStencil( L"Back Buffer Depth", bb->depthBuffer, bb->depthStencilView, bb->depthStencilState );
			m_backBuffer->AttachDepthStencil(bbDepth);

			//create a default viewport for the back buffer
			m_backBuffer->CreateViewport( L"Back Buffer Viewport", 0 );

			m_activeRenderTarget = m_backBuffer;

			m_name = name;
			m_renderWindow = window;

			//create a render state manager to manage depth stencils, blend states and raster states
			m_rasterStateManager = new Kiwi::RasterStateManager( this );
		
		}catch(...)
		{
			SAFE_DELETE(m_d3dInterface);
			throw;
		}

	}

	Renderer::~Renderer()
	{
		
		m_activeRenderTarget = 0;
		m_renderWindow = 0;

		//SAFE_DELETE(m_rasterStateManager);
		SAFE_DELETE( m_backBuffer );
		SAFE_DELETE(m_d3dInterface);

	}

	void Renderer::Present()
	{

		assert( m_d3dInterface != 0 && m_renderWindow != 0 );

		m_d3dInterface->Present( m_renderWindow->GetVSyncEnabled() );

	}

	//Kiwi::RenderTarget* Renderer::CreateRenderTarget(std::wstring name, const Kiwi::Vector2& dimensions, ID3D11Texture2D* tex)
	//{

	//	assert( m_d3dInterface != 0 );

	//	try
	//	{

	//		ID3D11Texture2D* texture = tex;
	//		ID3D11RenderTargetView* view = 0;
	//		ID3D11ShaderResourceView* shaderRes = 0;

	//		D3D11_TEXTURE2D_DESC textureDesc;
	//		D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
	//		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	//		HRESULT hr;

	//		ZeroMemory(&textureDesc, sizeof(textureDesc));

	//		if(texture == 0)
	//		{
	//			//create a new texture
	//			textureDesc.Width = (unsigned int)dimensions.x;
	//			textureDesc.Height = (unsigned int)dimensions.y;
	//			textureDesc.MipLevels = 1;
	//			textureDesc.ArraySize = 1;
	//			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//			textureDesc.SampleDesc.Count = 1;
	//			textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//			textureDesc.CPUAccessFlags = 0;
	//			textureDesc.MiscFlags = 0;

	//			hr = m_d3dInterface->GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
	//			if(FAILED(hr))
	//			{
	//				throw Kiwi::Exception(L"D3D11Interface::CreateRenderTarget", L"Failed to create render target texture");
	//			}
	//		}else
	//		{
	//			//a pre-existing texture was passed in (e.g. back buffer) so just use it
	//			texture->GetDesc(&textureDesc);
	//		}

	//		rtViewDesc.Format = textureDesc.Format;
	//		rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//		rtViewDesc.Texture2D.MipSlice = 0;

	//		hr = m_d3dInterface->GetDevice()->CreateRenderTargetView(texture, &rtViewDesc, &view);
	//		if(FAILED(hr))
	//		{
	//			SAFE_RELEASE(texture);

	//			throw Kiwi::Exception(L"D3D11Interface::CreateRenderTarget", L"Failed to create render target view");
	//		}

	//		shaderResourceViewDesc.Format = textureDesc.Format;
	//		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	//		shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//		hr = m_d3dInterface->GetDevice()->CreateShaderResourceView(texture, &shaderResourceViewDesc, &shaderRes);
	//		if(FAILED(hr))
	//		{
	//			SAFE_RELEASE(texture);
	//			SAFE_RELEASE(view);

	//			throw Kiwi::Exception(L"D3D11Interface::CreateRenderTarget", L"Failed to create render target shader resource");
	//		}

	//		Kiwi::RenderTarget* newRT = new Kiwi::RenderTarget(name, texture, view, shaderRes);

	//		return newRT;

	//	}catch(...)
	//	{
	//		throw;
	//	}

	//}

	/*clears the currently active render target, including its depth stencil*/
	void Renderer::ClearRenderTarget(const Kiwi::Vector4& color)
	{

		assert( m_d3dInterface != 0 );

		if(m_activeRenderTarget == 0) return;

		float col[4] = { color.x, color.y, color.z, color.w };

		m_d3dInterface->GetDeviceContext()->ClearRenderTargetView(m_activeRenderTarget->GetView(), col);

		Kiwi::DepthStencil* stencil = m_activeRenderTarget->GetDepthStencil();
		if(stencil != 0 && stencil->GetD3DView() != 0 )
		{
			m_d3dInterface->GetDeviceContext()->ClearDepthStencilView(stencil->GetD3DView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

	}

	/*clears the passed render target as well as its depth stencil*/
	void Renderer::ClearRenderTarget(Kiwi::RenderTarget* rt, const Kiwi::Vector4& color)
	{

		assert( m_d3dInterface != 0 );

		if(rt == 0) return;

		float col[4] = { color.x, color.y, color.z, color.w };

		m_d3dInterface->GetDeviceContext()->ClearRenderTargetView(rt->GetView(), col);

		Kiwi::DepthStencil* stencil = rt->GetDepthStencil();
		if(stencil != 0 && stencil->GetD3DView() != 0 )
		{
			m_d3dInterface->GetDeviceContext()->ClearDepthStencilView(stencil->GetD3DView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

	}

	Kiwi::RasterState* Renderer::CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc )
	{

		assert( m_rasterStateManager != 0 );

		return m_rasterStateManager->CreateRasterState( name, desc );

	}

	void Renderer::SetPrimitiveTopology(Kiwi::PrimitiveTopology topology)
	{

		assert( m_d3dInterface != 0 );

		try
		{
			m_d3dInterface->SetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)topology);
		}catch(...)
		{
			throw;
		}

	}

	void Renderer::SetRenderTarget(Kiwi::RenderTarget* renderTarget)
	{

		assert( m_d3dInterface != 0 );

		if(renderTarget)
		{

			m_d3dInterface->SetRenderTarget( renderTarget->GetView(), NULL );

			Kiwi::DepthStencil* depthState = renderTarget->GetDepthStencil();
			Kiwi::BlendState* blendState = renderTarget->GetBlendState();

			if( depthState != 0 )
			{
				m_d3dInterface->SetRenderTarget( renderTarget->GetView(), depthState->GetD3DView() );

			} else
			{
				m_d3dInterface->SetRenderTarget( renderTarget->GetView(), NULL );
			}

			this->SetBlendState( blendState );

			m_activeRenderTarget = renderTarget;
		}

	}

	void Renderer::SetBackBufferRenderTarget()
	{

		assert( m_d3dInterface != 0 );

		this->SetRenderTarget( m_backBuffer );

	}

	void Renderer::SetRasterState( Kiwi::RasterState* rasterState )
	{

		if( rasterState )
		{
			m_d3dInterface->SetRasterState( rasterState->GetD3DRasterState() );
		}

	}

	void Renderer::SetRasterState( std::wstring name )
	{

		assert( m_rasterStateManager != 0 );

		this->SetRasterState( m_rasterStateManager->FindRasterStateWithName( name ) );

	}

	void Renderer::SetBlendState( Kiwi::BlendState* blendState )
	{

		if( blendState )
		{
			Kiwi::Vector4 bFactor = blendState->GetBlendFactor();
			float blendFactor[4] = { bFactor.x, bFactor.y, bFactor.z, bFactor.w };
			m_d3dInterface->GetDeviceContext()->OMSetBlendState( blendState->GetD3DBlendState(), blendFactor, blendState->GetSampleMask() );
		} else
		{
			//turn off blending if 0 is passed
			m_d3dInterface->GetDeviceContext()->OMSetBlendState( 0, 0, 0xffffffff );
		}

	}

	void Renderer::DestroyAllRasterStates()
	{

		assert( m_rasterStateManager != 0 );

		m_rasterStateManager->DestroyAllRasterStates();

	}

	void Renderer::DestroyRasterStateWithName( std::wstring name )
	{

		assert( m_rasterStateManager != 0 );

		m_rasterStateManager->DestroyRasterStateWithName( name );

	}

	DXGI_FORMAT Renderer::GetDXGIFormat()const
	{

		assert( m_d3dInterface != 0 );

		return m_d3dInterface->GetDXGIFormat();

	}

	Kiwi::MultisampleDesc Renderer::GetMultisampleDesc()
	{

		assert( m_d3dInterface != 0 );

		return Kiwi::MultisampleDesc{ m_d3dInterface->GetMultisampleCount(), m_d3dInterface->GetMultisampleQuality() };

	}

	ID3D11Device* Renderer::GetDevice()const
	{
		
		assert( m_d3dInterface != 0 );
		
		return m_d3dInterface->GetDevice();

		return 0;

	}

	ID3D11DeviceContext* Renderer::GetDeviceContext()const
	{
		
		assert( m_d3dInterface != 0 );

		return m_d3dInterface->GetDeviceContext();

		return 0;

	}

	Kiwi::RasterState* Renderer::FindRasterStateWithName( std::wstring name )
	{

		assert( m_rasterStateManager != 0 );

		return m_rasterStateManager->FindRasterStateWithName( name );

	}

};