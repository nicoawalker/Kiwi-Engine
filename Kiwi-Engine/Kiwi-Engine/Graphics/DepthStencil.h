#ifndef _KIWI_DEPTHSTENCIL_H_
#define _KIWI_DEPTHSTENCIL_H_

#include "DirectX.h"

#include "../Core/Vector2.h"

#include <string>

namespace Kiwi
{

	class Renderer;

	class DepthStencil
	{
	protected:

		std::wstring m_name;

		ID3D11DepthStencilState* m_depthState;
		ID3D11DepthStencilView* m_depthView;
		ID3D11Texture2D* m_depthBuffer;

	public:

		DepthStencil( std::wstring name, Kiwi::Renderer* renderer, const Kiwi::Vector2& dimensions, unsigned int mipLevels = 1, unsigned int arraySize = 1 );
		DepthStencil(std::wstring name, ID3D11Texture2D* depthBuffer, ID3D11DepthStencilView* depthView, ID3D11DepthStencilState* depthState);
		virtual ~DepthStencil();

		ID3D11Texture2D* GetD3DBuffer()const { return m_depthBuffer; }
		ID3D11DepthStencilState* GetD3DState()const { return m_depthState; }
		ID3D11DepthStencilView* GetD3DView()const { return m_depthView; }
		std::wstring GetName()const { return m_name; }

	};
};

#endif