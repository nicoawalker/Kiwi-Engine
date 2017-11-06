#include "BlendState.h"
#include "DirectX.h"

#include "..\Core\Utilities.h"
#include "../Core/Exception.h"

namespace Kiwi
{

	BlendState::BlendState( std::wstring name, ID3D11BlendState* blendState, unsigned int sampleMask )
	{
		if( blendState == 0 ) throw Kiwi::Exception( L"BlendState", L"[" + name + L"] Invalid blend state" );

		m_name = name;
		m_uid = Kiwi::NewUID();
		m_blendState = blendState;
		m_sampleMask = sampleMask;
	}

	BlendState::~BlendState()
	{
		SAFE_RELEASE( m_blendState );
	}
};