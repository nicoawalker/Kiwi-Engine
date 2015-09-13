#include "BlendState.h"
#include "DirectX.h"

#include "..\Core\Utilities.h"
#include "..\Core\Exception.h"

namespace Kiwi
{

	BlendState::BlendState( std::wstring name, ID3D11BlendState* blendState, const Kiwi::Vector4& blendFactor, unsigned int sampleMask )
	{
		
		if( blendState == 0 ) throw Kiwi::Exception( L"BlendState", L"[" + name + L"] Invalid blend state" );

		m_name = name;
		m_blendState = blendState;
		m_blendFactor = blendFactor;
		m_sampleMask = sampleMask;

	}

	BlendState::~BlendState()
	{

		SAFE_RELEASE( m_blendState );

	}
};