#ifndef _KIWIRASTERSTATE_H_
#define _KIWIRASTERSTATE_H_

#include "DirectX.h"

#include <string>

namespace Kiwi
{

	class Renderer;

	struct D3DRasterStateDesc
	{
		D3D11_RASTERIZER_DESC desc;

		D3DRasterStateDesc()
		{
			memset( &desc, 0, sizeof( D3D11_RASTERIZER_DESC ) );
		}
	};

	class RasterState
	{
	protected:

		std::wstring m_name;
		long m_uid;

		ID3D11RasterizerState* m_rasterState;

		Kiwi::D3DRasterStateDesc m_rasterDesc;

	public:

		RasterState( std::wstring name, Kiwi::Renderer* renderer, const Kiwi::D3DRasterStateDesc& desc );
		~RasterState();

		ID3D11RasterizerState* GetD3DRasterState()const { return m_rasterState; }

		std::wstring GetName()const { return m_name; }
		long GetUID()const { return m_uid; }

	};
};

#endif