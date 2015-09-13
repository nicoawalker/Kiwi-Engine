#include "DepthStencil.h"
#include "Renderer.h"

#include "../Core/Utilities.h"
#include "../Core/Assert.h"
#include "../Core/Exception.h"

namespace Kiwi
{

	DepthStencil::DepthStencil( std::wstring name, Kiwi::Renderer* renderer, const Kiwi::Vector2& dimensions, unsigned int mipLevels, unsigned int arraySize )
	{

		assert( renderer != 0 );

		m_name = name;

		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;

		//create depth buffer texture used for proper rendering in 3d space and allow for effects
		ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );
		depthBufferDesc.Width = (unsigned int)dimensions.x;
		depthBufferDesc.Height = (unsigned int)dimensions.y;
		depthBufferDesc.MipLevels = mipLevels;
		depthBufferDesc.ArraySize = arraySize;

		depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthBufferDesc.SampleDesc.Count = renderer->GetMultisampleDesc().count;
		depthBufferDesc.SampleDesc.Quality = renderer->GetMultisampleDesc().quality;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		HRESULT hr = renderer->GetDevice()->CreateTexture2D( &depthBufferDesc, NULL, &m_depthBuffer );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"DepthStencil", L"["+m_name+L"] Failed to create depth buffer" );
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

		hr = renderer->GetDevice()->CreateDepthStencilState( &depthStencilDesc, &m_depthState );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"DepthStencil", L"[" + m_name + L"] Failed to create depth stencil state" );
		}

		//setup the depth stencil buffer so DX will use the buffer as a depth stencil texture
		ZeroMemory( &dsViewDesc, sizeof( dsViewDesc ) );
		dsViewDesc.Format = depthBufferDesc.Format;
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsViewDesc.Texture2D.MipSlice = 0;

		hr = renderer->GetDevice()->CreateDepthStencilView( m_depthBuffer, &dsViewDesc, &m_depthView );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"DepthStencil", L"[" + m_name + L"] Failed to create depth stencil view" );
		}

	}

	DepthStencil::DepthStencil( std::wstring name, ID3D11Texture2D* depthBuffer, ID3D11DepthStencilView* depthView, ID3D11DepthStencilState* depthState )
	{

		m_name = name;

		m_depthState = depthState;
		m_depthBuffer = depthBuffer;
		m_depthView = depthView;

	}

	DepthStencil::~DepthStencil()
	{

		SAFE_RELEASE( m_depthState );
		SAFE_RELEASE( m_depthBuffer );
		SAFE_RELEASE( m_depthView );

	}

};