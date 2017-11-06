#ifndef _KIWI_BLENDSTATE_H_
#define _KIWI_BLENDSTATE_H_

#include "DirectX.h"

#include <string>

struct ID3D11BlendState;

namespace Kiwi
{

	class Renderer;

	struct KiwiBlendDesc
	{

		D3D11_RENDER_TARGET_BLEND_DESC desc;
		bool alphaToCoverageEnable;
		bool independentBlendEnable;

		KiwiBlendDesc()
		{
			memset( &desc, 0, sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		}
	};

	class BlendState
	{
	protected:

		std::wstring m_name;
		long m_uid;

		ID3D11BlendState* m_blendState;

		unsigned int m_sampleMask;

	public:

		BlendState( std::wstring name, ID3D11BlendState* blendState, unsigned int sampleMask = 0xFFFFFFFF );
		virtual ~BlendState();

		std::wstring GetName()const { return m_name; }
		long GetUID()const { return m_uid; }

		ID3D11BlendState* GetD3DBlendState()const { return m_blendState; }

		unsigned int GetSampleMask()const { return m_sampleMask; }

	};

};

#endif