#ifndef _KIWI_DEPTHSTENCIL_H_
#define _KIWI_DEPTHSTENCIL_H_

#include "DirectX.h"

#include "../Core/Vector2.h"

#include <string>

namespace Kiwi
{

	class Renderer;

	struct DepthBufferProperties
	{
		int width; //width in pixels
		int height; //height in pixels
		int mipLevels;
		int arraySize;
		DXGI_FORMAT format; //possible values: https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
		int sampleCount;
		int sampleQuality;
		D3D11_USAGE usage; //possible values: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
		unsigned int cpuAccessFlags;
		unsigned int miscFlags;

		/*use default depth buffer properties*/
		DepthBufferProperties()
		{
			width = -1; //copy from back buffer
			height = -1; //copy from back buffer
			mipLevels = 1;
			arraySize = 1;
			format = DXGI_FORMAT_D32_FLOAT;
			sampleCount = -1; //copy from back buffer
			sampleQuality = -1; //copy from back buffer
			usage = D3D11_USAGE_DEFAULT;
			cpuAccessFlags = 0;
			miscFlags = 0;
		}

		/*manually set depth buffer properties*/
		DepthBufferProperties( unsigned int width, unsigned int height, unsigned int mipLevels = 1, unsigned int arraySize = 1, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT,
							   unsigned int sampleCount = 1, unsigned int sampleQualtiy = 1, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, unsigned int cpuAccessFlags = 0, unsigned int miscFlags = 0 )
		{
			this->width = width;
			this->height = height;
			this->mipLevels = mipLevels;
			this->arraySize = arraySize;
			this->format = format;
			this->sampleCount = sampleCount;
			this->sampleQuality = sampleQuality;
			this->usage = usage;
			this->cpuAccessFlags = cpuAccessFlags;
			this->miscFlags = miscFlags;
		}
	};

	struct DepthStencilProperties
	{
		bool enableDepth;
		bool enableStencil;
		D3D11_DEPTH_WRITE_MASK depthWriteMask;
		D3D11_COMPARISON_FUNC depthFunction;
		unsigned char stencilReadMask;
		unsigned char stencilWriteMask;
		D3D11_STENCIL_OP frontFailOp;
		D3D11_STENCIL_OP frontDepthFailOp;
		D3D11_STENCIL_OP frontPassOp;
		D3D11_COMPARISON_FUNC frontStencilFunc;
		D3D11_STENCIL_OP backFailOp;
		D3D11_STENCIL_OP backDepthFailOp;
		D3D11_STENCIL_OP backPassOp;
		D3D11_COMPARISON_FUNC backStencilFunc;

		/*use default depth stencil properties*/
		DepthStencilProperties()
		{
			/*depth test parameters*/
			enableDepth = true;
			depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthFunction = D3D11_COMPARISON_LESS;

			/*stencil test parameters*/
			enableStencil = true;
			stencilReadMask = 0xFF;
			stencilWriteMask = 0xFF;

			/*stencil operations if pixel is front facing*/
			frontFailOp = D3D11_STENCIL_OP_KEEP;
			frontDepthFailOp = D3D11_STENCIL_OP_INCR;
			frontPassOp = D3D11_STENCIL_OP_KEEP;
			frontStencilFunc = D3D11_COMPARISON_ALWAYS;

			/*stencil operations if pixel is back-facing*/
			backFailOp = D3D11_STENCIL_OP_KEEP;
			backDepthFailOp = D3D11_STENCIL_OP_DECR;
			backPassOp = D3D11_STENCIL_OP_KEEP;
			backStencilFunc = D3D11_COMPARISON_ALWAYS;
		}

		DepthStencilProperties( bool enableDepth, bool enableStencil, D3D11_DEPTH_WRITE_MASK depthWriteMask, D3D11_COMPARISON_FUNC depthFunction, unsigned char stencilReadMask,
								unsigned char stencilWriteMask, D3D11_STENCIL_OP frontFailOp, D3D11_STENCIL_OP frontDepthFailOp, D3D11_STENCIL_OP frontPassOp, D3D11_COMPARISON_FUNC frontStencilFunc,
								D3D11_STENCIL_OP backFailOp, D3D11_STENCIL_OP backDepthFailOp, D3D11_STENCIL_OP backPassOp, D3D11_COMPARISON_FUNC backStencilFunc )
		{
			this->enableDepth = enableDepth;
			this->enableStencil = enableStencil;
			this->depthWriteMask = depthWriteMask;
			this->depthFunction = depthFunction;
			this->stencilReadMask = stencilReadMask;
			this->stencilWriteMask = stencilWriteMask;
			this->frontFailOp = frontFailOp;
			this->frontDepthFailOp = frontDepthFailOp;
			this->frontPassOp = frontPassOp;
			this->frontStencilFunc = frontStencilFunc;
			this->backFailOp = backFailOp;
			this->backDepthFailOp = backDepthFailOp;
			this->backPassOp = backPassOp;
			this->backStencilFunc = backStencilFunc;
		}
	};

	class DepthStencil
	{
	protected:

		std::wstring m_name;
		long m_uid;

		ID3D11DepthStencilState* m_depthDisabledState;
		ID3D11DepthStencilState* m_depthState;
		ID3D11DepthStencilView* m_depthView;
		ID3D11Texture2D* m_depthBuffer;

	public:

		DepthStencil( std::wstring name, ID3D11Texture2D* depthBuffer, ID3D11DepthStencilView* depthView, ID3D11DepthStencilState* depthState, ID3D11DepthStencilState* depthDisabledState );
		virtual ~DepthStencil();

		ID3D11Texture2D* GetD3DBuffer()const { return m_depthBuffer; }
		ID3D11DepthStencilState* GetD3DState()const { return m_depthState; }
		ID3D11DepthStencilState* GetD3DDisabledState()const { return m_depthDisabledState; }
		ID3D11DepthStencilView* GetD3DView()const { return m_depthView; }
		std::wstring GetName()const { return m_name; }
		long GetUID()const { return m_uid; }

	};
};

#endif