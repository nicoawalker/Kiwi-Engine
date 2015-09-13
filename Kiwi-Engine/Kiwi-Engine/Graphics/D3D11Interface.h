#ifndef _KIWI_D3D11INTERFACE_H_
#define _KIWI_D3D11INTERFACE_H_

#include "DirectX.h"

#include "../Core/Vector4.h"

#include <string>

namespace Kiwi
{

	struct VideoCardInfo
	{
		int totalRAM;
		std::wstring name;
	};

	struct BackBuffer
	{
		ID3D11Texture2D* texture;
		ID3D11RenderTargetView* renderTargetView;
		ID3D11Texture2D* depthBuffer;
		ID3D11DepthStencilState* depthStencilState;
		ID3D11DepthStencilView* depthStencilView;
	};

	class D3D11Interface
	{
	private:

		IDXGISwapChain* m_swapChain;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;

		/*ID3D11Texture2D* m_backBuffer;
		ID3D11RenderTargetView* m_backBufferRenderTargetView;
		ID3D11Texture2D* m_backBufferDepthBuffer;
		ID3D11DepthStencilState* m_backBufferDepthState;
		ID3D11DepthStencilView* m_backBufferDepthView;*/

		BackBuffer m_backBuffer;

		unsigned int m_multisampleCount;
		unsigned int m_multisampleQuality;

		DXGI_FORMAT m_dxgiFormat;

		//ID3D11Texture2D* m_depthStencilBuffer;
		/*ID3D11DepthStencilState* m_depthStencilState;
		ID3D11DepthStencilState* m_depthDisabledState;
		ID3D11DepthStencilView* m_depthStencilView;*/

		bool m_vSyncEnabled;
		bool m_fullscreen;

		Kiwi::VideoCardInfo m_videoCardInfo;

	public:

		D3D11Interface(HWND hwnd, bool fullscreen, unsigned int width, unsigned int height, bool vsyncEnabled);
		~D3D11Interface();

		void Present(bool vsyncEnabled);

		/*sets the active render target and depth stencil*/
		void SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView);
		
		/*sets the back buffer to be the active render target*/
		void SetBackBufferRenderTarget();

		void ClearBackBuffer(float r, float g, float b, float a);

		void SetRasterState(ID3D11RasterizerState* state);
		void SetBlendState(ID3D11BlendState* blendState, const Kiwi::Vector4& blendFactor, UINT sampleMask);

		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology);

		unsigned int GetMultisampleQuality()const { return m_multisampleQuality; }
		unsigned int GetMultisampleCount()const { return m_multisampleCount; }

		DXGI_FORMAT GetDXGIFormat()const { return m_dxgiFormat; }
		ID3D11Device* GetDevice()const { return m_device; }
		ID3D11DeviceContext* GetDeviceContext()const { return m_deviceContext; }
		const VideoCardInfo& GetVideoCardInfo()const { return m_videoCardInfo; }
		Kiwi::BackBuffer* GetBackBuffer() { return &m_backBuffer; }

	};
};

#endif