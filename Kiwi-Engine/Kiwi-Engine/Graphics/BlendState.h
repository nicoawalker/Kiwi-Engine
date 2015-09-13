#ifndef _KIWI_BLENDSTATE_H_
#define _KIWI_BLENDSTATE_H_

#include "..\Core\Vector4.h"

#include <string>

struct ID3D11BlendState;

namespace Kiwi
{

	class Renderer;

	class BlendState
	{
	protected:

		std::wstring m_name;

		ID3D11BlendState* m_blendState;

		Kiwi::Vector4 m_blendFactor;

		unsigned int m_sampleMask;

	public:

		BlendState( std::wstring name, ID3D11BlendState* blendState, const Kiwi::Vector4& blendFactor, unsigned int sampleMask = 0xFFFFFF );
		virtual ~BlendState();

		std::wstring GetName()const { return m_name;}

		ID3D11BlendState* GetD3DBlendState()const { return m_blendState; }
		
		const Kiwi::Vector4& GetBlendFactor()const { return m_blendFactor; }

		unsigned int GetSampleMask()const { return m_sampleMask; }

	};

};

#endif