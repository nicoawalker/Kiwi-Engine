#include "D3D11Interface.h"

#include "..\Types.h"

#include "../Core/Exception.h"
#include "..\Core\Utilities.h"
#include "../Core/Vector4d.h"

namespace Kiwi
{

	D3D11Interface::D3D11Interface( ) :
		m_dcUniqueLock( m_deviceContextMutex, std::defer_lock )
	{

		m_deviceContextAcquired = false;
		m_backBuffer.depthBuffer = 0;
		m_backBuffer.depthStencilState = 0;
		m_backBuffer.depthStencilView = 0;
		m_backBuffer.renderTargetView = 0;
		m_backBuffer.texture = 0;

		m_multisampleCount = 1;
		m_multisampleQuality = 0;

		m_fullscreen = false;
		m_vSyncEnabled = false;

		//set the color format to use for the buffers
		m_dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		m_device = 0;
		m_deviceContext = 0;
		m_swapChain = 0;

	}

	void D3D11Interface::Initialize( HWND hwnd, bool fullscreen, unsigned int width, unsigned int height, bool vsyncEnabled, uint multiSampleCount, uint multiSampleQuality )
	{

		IDXGIFactory* factory = 0;
		IDXGIAdapter* adapter = 0;
		IDXGIOutput* adapterOutput = 0;
		DXGI_MODE_DESC* displayModeList = 0;
		DXGI_ADAPTER_DESC adapterDesc;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		D3D_FEATURE_LEVEL featureLevel;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_DESC depthDisabledDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
		unsigned int numModes;
		unsigned int numerator = 0, denominator = 1;

		m_deviceContextAcquired = false;
		m_backBuffer.depthBuffer = 0;
		m_backBuffer.depthStencilState = 0;
		m_backBuffer.depthStencilView = 0;
		m_backBuffer.renderTargetView = 0;
		m_backBuffer.texture = 0;

		m_multisampleCount = multiSampleCount;
		m_multisampleQuality = multiSampleQuality;

		m_fullscreen = fullscreen;
		m_vSyncEnabled = vsyncEnabled;

		//set the color format to use for the buffers
		m_dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		std::lock_guard<std::mutex> dcGuard( m_deviceContextMutex );
		std::lock_guard<std::mutex> scGuard( m_swapChainMutex );

		//get the monitor refresh rate to prevent performance issues
		//create a DX graphics interface factory
		HRESULT m_lastError = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)&factory );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create graphics interface factory" );
		}
		//now use it to create an adapter for the primary video card
		m_lastError = factory->EnumAdapters( 0, &adapter );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create graphics adapter" );
		}
		//enumerate the primary monitor
		m_lastError = adapter->EnumOutputs( 0, &adapterOutput );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to enumerate primary monitor" );
		}
		//get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM format
		m_lastError = adapterOutput->GetDisplayModeList( m_dxgiFormat, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to get display mode list" );
		}
		//create a list of all the display modes for the monitor/video card
		displayModeList = new DXGI_MODE_DESC[numModes];

		//fill the display mode lists
		m_lastError = adapterOutput->GetDisplayModeList( m_dxgiFormat, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to fill display mode list" );
		}

		//find the correct display mode
		for( unsigned int i = 0; i < numModes; i++ )
		{
			if( displayModeList[i].Width == width && displayModeList[i].Height == height )
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}

		//lastly, get the name of the video card and the amount of RAM
		m_lastError = adapter->GetDesc( &adapterDesc );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to get adapter info" );
		}
		//store video card RAM in megabytes
		m_videoCardInfo.totalRAM = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		//get the name of the video card
		m_videoCardInfo.name = adapterDesc.Description;

		delete[] displayModeList;
		displayModeList = 0;

		SAFE_RELEASE( adapterOutput );
		SAFE_RELEASE( adapter );
		SAFE_RELEASE( factory );

		//now initialize the swapchain/desc
		ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );

		swapChainDesc.BufferCount = 1; //single back buffer
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = m_dxgiFormat; //32-bit surface

		if( vsyncEnabled )
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		} else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		//using the back buffer as a render target
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;

		//turn off multisampling
		swapChainDesc.SampleDesc.Count = m_multisampleCount; //number of multisamples per pixel, sample count > 1 enables MSAA
		swapChainDesc.SampleDesc.Quality = m_multisampleQuality;

		swapChainDesc.Windowed = !fullscreen;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //discard back buffer contents after presenting
		swapChainDesc.Flags = 0;

		//using DX 11
		featureLevel = D3D_FEATURE_LEVEL_11_0;

		//create the swapchain, direct3d device, and d3d context
#ifdef KIWIDEBUGENABLED
		m_lastError = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext );
#else
		m_lastError = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext );
#endif
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create device/swap chain (" + Kiwi::GetD3DErrorString( m_lastError ) + L")", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		//get the back buffer and attach it to the swap chain
		m_lastError = m_swapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBuffer.texture );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to attach back buffer" );
		}

		//create render target view
		m_lastError = m_device->CreateRenderTargetView( m_backBuffer.texture, NULL, &m_backBuffer.renderTargetView );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create back buffer render target view", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		//create depth buffer texture to allow proper rendering in 3d space and allow for effects
		ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthBufferDesc.SampleDesc.Count = m_multisampleCount;
		depthBufferDesc.SampleDesc.Quality = m_multisampleQuality;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		m_lastError = m_device->CreateTexture2D( &depthBufferDesc, NULL, &m_backBuffer.depthBuffer );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create back buffer depth stencil buffer" );;
		}

		//now setup depth stencil to allow control of what type of depth test is done for each pixel
		ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		//for front-facing pixels
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//for back facing pixels
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		m_lastError = m_device->CreateDepthStencilState( &depthStencilDesc, &m_backBuffer.depthStencilState );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create back buffer depth stencil state" );
		}

		//create the depth disabled state
		ZeroMemory( &depthDisabledDesc, sizeof( depthDisabledDesc ) );
		depthDisabledDesc.DepthEnable = false;
		depthDisabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDisabledDesc.StencilEnable = true;
		depthDisabledDesc.StencilReadMask = 0xFF;
		depthDisabledDesc.StencilWriteMask = 0xFF;
		//for front-facing pixels
		depthDisabledDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//for back facing pixels
		depthDisabledDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		m_lastError = m_device->CreateDepthStencilState( &depthDisabledDesc, &m_backBuffer.depthDisabledState );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create back buffer depth stencil state" );
		}

		//set the depth stencil state
		//m_deviceContext->OMSetDepthStencilState(m_backBuffer.depthStencilState, 1);

		//setup the depth stencil buffer so DX will use the buffer as a depth stencil texture
		ZeroMemory( &dsViewDesc, sizeof( dsViewDesc ) );
		dsViewDesc.Format = depthBufferDesc.Format;
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsViewDesc.Texture2D.MipSlice = 0;

		m_lastError = m_device->CreateDepthStencilView( m_backBuffer.depthBuffer, &dsViewDesc, &m_backBuffer.depthStencilView );
		if( FAILED( m_lastError ) )
		{
			throw Kiwi::Exception( L"D3D11Interface::Initialize", L"Failed to create back buffer depth stencil view" );
		}

		//now bind the render target view and depth stencil buffer to the render pipeline
		//m_deviceContext->OMSetRenderTargets(1, &m_backBuffer.renderTargetView, m_backBuffer.depthStencilView);

		//by default, set the primitive type to triangle list
		m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	}

	D3D11Interface::~D3D11Interface()
	{
		/*ID3D11Debug* db = nullptr;
		m_device->QueryInterface( __uuidof(ID3D11Debug), reinterpret_cast<void**>(&db) );
		if( db != nullptr )
		{
			db->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
		}*/

		if( m_swapChain )
		{
			//get out of full screen mode first to avoid exceptions being thrown
			m_swapChain->SetFullscreenState( false, NULL );
		}

		SAFE_RELEASE( m_backBuffer.texture );
		SAFE_RELEASE( m_backBuffer.renderTargetView );

		SAFE_RELEASE( m_deviceContext );
		SAFE_RELEASE( m_device );
		SAFE_RELEASE( m_swapChain );

	}

	void D3D11Interface::Shutdown()
	{
		if( m_swapChain )
		{
			//get out of full screen mode first to avoid exceptions being thrown
			m_swapChain->SetFullscreenState( false, NULL );
		}

		SAFE_RELEASE( m_backBuffer.texture );
		SAFE_RELEASE( m_backBuffer.renderTargetView );

		SAFE_RELEASE( m_deviceContext );
		SAFE_RELEASE( m_device );
		SAFE_RELEASE( m_swapChain );
	}

	void D3D11Interface::DrawIndexed( unsigned int indexCount, unsigned int startIndex, int baseVertex )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::DrawIndexed", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->DrawIndexed( indexCount, startIndex, baseVertex );
	}

	void D3D11Interface::DrawIndexedInstanced( unsigned int indexCountPerInstance, unsigned instanceCount, unsigned int indexStart, int baseVertexLocation, unsigned int instanceStart )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::DrawIndexedInstanced", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->DrawIndexedInstanced( indexCountPerInstance, instanceCount, indexStart, baseVertexLocation, instanceStart );

	}

	void D3D11Interface::Present( bool vsyncEnabled )
	{

		if( m_swapChain == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::Present", L"Swap chain is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}


		std::lock_guard<std::mutex> guard( m_swapChainMutex );
		if( vsyncEnabled )
		{
			m_swapChain->Present( 0, 0 );

		} else
		{
			m_swapChain->Present( 0, 0 );
		}

	}

	HRESULT D3D11Interface::MapResource( ID3D11Resource* buffer, unsigned int subResource, D3D11_MAP mapType, unsigned int flags, D3D11_MAPPED_SUBRESOURCE* resource )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::MapResource", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		return m_deviceContext->Map( buffer, subResource, mapType, flags, resource );

	}

	void D3D11Interface::UnmapResource( ID3D11Resource* resource, unsigned int subResource )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::UnmapResource", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( resource )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->Unmap( resource, subResource );
		}
	}

	void D3D11Interface::CopyResource( ID3D11Resource* destination, ID3D11Resource* source )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::CopyResource", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );

		m_deviceContext->CopyResource( destination, source );
	}

	void D3D11Interface::UpdateSubresource( ID3D11Resource* resource, unsigned int subResource, const D3D11_BOX* destBox, const void* data, unsigned int srcRowPitch, unsigned int srcDepthPitch )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::UpdateSubresource", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( resource )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->UpdateSubresource( resource, subResource, destBox, data, srcRowPitch, srcDepthPitch );
		}
	}

	void D3D11Interface::SetRenderTarget( ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetRenderTarget", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( renderTarget && depthStencilView )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->OMSetRenderTargets( 1, &renderTarget, depthStencilView );
		}
	}

	void D3D11Interface::SetBackBufferRenderTarget()
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetBackBufferRenderTarget", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->OMSetRenderTargets( 1, &m_backBuffer.renderTargetView, m_backBuffer.depthStencilView );
	}

	void D3D11Interface::SetDepthStencilState( ID3D11DepthStencilState* depthStencil )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetDepthStencilState", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->OMSetDepthStencilState( depthStencil, 1 );
	}

	void D3D11Interface::ClearBackBuffer( float r, float g, float b, float a )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::ClearBackBuffer", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		float color[4] = { r, g, b, a };

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->ClearRenderTargetView( m_backBuffer.renderTargetView, color );

		if( m_backBuffer.depthStencilView )
		{
			m_deviceContext->ClearDepthStencilView( m_backBuffer.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		}
	}

	void D3D11Interface::ClearDepthStencilView( ID3D11DepthStencilView* view, unsigned int clearFlags, float depth, unsigned char stencil )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::ClearDepthStencilView", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->ClearDepthStencilView( view, clearFlags, depth, stencil );

	}

	void D3D11Interface::ClearRenderTargetView( ID3D11RenderTargetView* rtView, const float* clearColor )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::ClearRenderTargetView", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( rtView )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->ClearRenderTargetView( rtView, clearColor );
		}

	}

	HRESULT D3D11Interface::CreateBuffer( const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* srData, ID3D11Buffer** buffer )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::CreateBuffer", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( bufferDesc )
		{
			HRESULT hr = m_device->CreateBuffer( bufferDesc, srData, buffer );

			return hr;
		}
		
		return 0;

	}

	HRESULT D3D11Interface::CreateSampler( D3D11_SAMPLER_DESC* samplerDesc, ID3D11SamplerState** sampler )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::CreateSampler", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( samplerDesc )
		{
			HRESULT hr = m_device->CreateSamplerState( samplerDesc, sampler );

			return hr;
		}

		return 0;

	}

	HRESULT D3D11Interface::CreateInputLayout( ID3D10Blob* vertexBlob, D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int layoutSize, ID3D11InputLayout** layout )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::CreateInputLayout", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( vertexBlob && polygonLayout )
		{
			HRESULT hr = m_device->CreateInputLayout( polygonLayout, layoutSize, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), layout );

			return hr;
		}

		return 0;

	}

	void D3D11Interface::SetRasterState( ID3D11RasterizerState* state )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetRasterState", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( state )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->RSSetState( state );
		}
	}

	void D3D11Interface::SetBlendState()
	{
		if( m_deviceContext == nullptr )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetBlendState", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		//disable blending
		m_deviceContext->OMSetBlendState( 0, 0, 0xFFFFFFFF );
	}

	void D3D11Interface::SetBlendState( ID3D11BlendState* blendState, const Kiwi::Vector4d& blendFactor, UINT sampleMask )
	{
		if( m_deviceContext == nullptr )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetBlendState", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		if( blendState == nullptr )
		{
			//disable blending
			m_deviceContext->OMSetBlendState( 0, 0, 0xFFFFFFFF );

		} else
		{
			float factor[] = { (float)blendFactor.x, (float)blendFactor.y, (float)blendFactor.z, (float)blendFactor.w };
			m_deviceContext->OMSetBlendState( blendState, factor, sampleMask );
		}
	}

	void D3D11Interface::SetViewports( D3D11_VIEWPORT viewports[], unsigned int numViewports )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetViewports", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( viewports != 0 )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->RSSetViewports( numViewports, viewports );
		}
	}

	void D3D11Interface::SetVertexBuffers( unsigned int startSlot, unsigned int numBuffers, ID3D11Buffer** buffers, unsigned int* stride, unsigned int* offset )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetVertexBuffers", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( buffers )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->IASetVertexBuffers( startSlot, numBuffers, buffers, stride, offset );
		}
	}

	void D3D11Interface::SetIndexBuffer( ID3D11Buffer* buffer, DXGI_FORMAT format, unsigned int offset )
	{
		if( m_deviceContext == nullptr )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetIndexBuffer", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		if( buffer != nullptr )
		{
			std::lock_guard<std::mutex> guard( m_deviceContextMutex );
			m_deviceContext->IASetIndexBuffer( buffer, format, offset );
		}
	}

	void D3D11Interface::SetVertexShader( ID3D11VertexShader* vShader, ID3D11ClassInstance** classInstances, unsigned int numClassInstances )
	{
		if( m_deviceContext == nullptr )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetVertexShader", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->VSSetShader( vShader, classInstances, numClassInstances );
	}

	void D3D11Interface::SetPixelShader( ID3D11PixelShader* pShader, ID3D11ClassInstance** classInstances, unsigned int numClassInstances )
	{
		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetPixelShader", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->PSSetShader( pShader, classInstances, numClassInstances );
	}

	void D3D11Interface::SetInputLayout( ID3D11InputLayout* layout )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetInputLayout", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->IASetInputLayout( layout );

	}

	void D3D11Interface::SetVertexShaderCBuffers( unsigned int startSlot, unsigned int numBuffers, ID3D11Buffer** buffers )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetVertexShaderCBuffers", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->VSSetConstantBuffers( startSlot, numBuffers, buffers );

	}

	void D3D11Interface::SetPixelShaderCBuffers( unsigned int startSlot, unsigned int numBuffers, ID3D11Buffer** buffers )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetPixelShaderCBuffers", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->PSSetConstantBuffers( startSlot, numBuffers, buffers );

	}

	void D3D11Interface::SetShaderSamplers( unsigned int startSlot, unsigned int numSamplers, ID3D11SamplerState** samplers )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetShaderSamplers", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->PSSetSamplers( startSlot, numSamplers, samplers );

	}

	void D3D11Interface::SetPixelShaderResources( unsigned int startSlot, unsigned int numViews, ID3D11ShaderResourceView** resources )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetPixelShaderResources", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->PSSetShaderResources( startSlot, numViews, resources );

	}

	/*unbinds all bound pixel shader resources*/
	void D3D11Interface::FreePixelShaderResources()
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetPixelShaderResources", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );

		ID3D11ShaderResourceView* srvs[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
		m_deviceContext->PSSetShaderResources( 0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, srvs );

	}

	/*unbinds a certain number of pixel shader resources starting from a certain slot
	>startSlot: the starting slot to being clearing from
	>numResources: the number of resources, including the start slot, to erase valid values are from 1 to D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - startSlot*/
	void D3D11Interface::FreePixelShaderResources( unsigned int startSlot, unsigned int numResources )
	{

		if( m_deviceContext == 0 )
		{
			throw Kiwi::Exception( L"D3D11Interface::SetPixelShaderResources", L"Device context is NULL", KIWI_EXCEPTION::NULLOPERATION );
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );

		ID3D11ShaderResourceView* srvs[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
		m_deviceContext->PSSetShaderResources( startSlot, numResources, srvs );

	}

	void D3D11Interface::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	{

		if(m_deviceContext == 0)
		{
			throw Kiwi::Exception(L"D3D11Interface::SetPrimitiveTopology", L"Device context is NULL");
			return;
		}

		std::lock_guard<std::mutex> guard( m_deviceContextMutex );
		m_deviceContext->IASetPrimitiveTopology(topology);

	}

};