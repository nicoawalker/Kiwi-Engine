#include "DepthStencil.h"
#include "Renderer.h"

#include "../Core/Utilities.h"
#include "..\Core\Assert.h"
#include "../Core/Exception.h"

namespace Kiwi
{

	DepthStencil::DepthStencil( std::wstring name, ID3D11Texture2D* depthBuffer, ID3D11DepthStencilView* depthView, ID3D11DepthStencilState* depthState, ID3D11DepthStencilState* depthDisabledState )
	{
		m_name = name;
		m_uid = Kiwi::NewUID();

		m_depthState = depthState;
		m_depthBuffer = depthBuffer;
		m_depthView = depthView;
		m_depthDisabledState = depthDisabledState;
	}

	DepthStencil::~DepthStencil()
	{
		SAFE_RELEASE( m_depthState );
		SAFE_RELEASE( m_depthBuffer );
		SAFE_RELEASE( m_depthView );
		SAFE_RELEASE( m_depthDisabledState );
	}

};