#ifndef _KIWI_D3D11INTERFACE_H_
#define _KIWI_D3D11INTERFACE_H_

#include "DirectX.h"

#include "../Types.h"

#include "..\Core\Utilities.h"

#include <string>
#include <mutex>
#include <unordered_map>

namespace Kiwi
{

	class Vector4d;

	struct VideoCardInfo
	{
		int totalRAM;
		std::wstring name;
	};

	struct BackBuffer
	{
		ID3D11Texture2D* texture;
		ID3D11RenderTargetView* renderTargetView;

		/*managed and released by Kiwi::DepthStencil objects*/
		ID3D11Texture2D* depthBuffer;
		ID3D11DepthStencilState* depthStencilState;
		ID3D11DepthStencilState* depthDisabledState;
		ID3D11DepthStencilView* depthStencilView;
	};

	class D3D11Interface
	{
	private:

		IDXGISwapChain* m_swapChain;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;

		BackBuffer m_backBuffer;

		unsigned int m_multisampleCount;
		unsigned int m_multisampleQuality;

		std::unordered_map<uint, uint> m_supportedSampleQualities;

		DXGI_FORMAT m_dxgiFormat;

		bool m_vSyncEnabled;
		bool m_fullscreen;

		Kiwi::VideoCardInfo m_videoCardInfo;

		//mutex used for the device context (which is not thread-safe)
		std::mutex m_deviceContextMutex;
		std::unique_lock<std::mutex> m_dcUniqueLock;
		bool m_deviceContextAcquired;

		std::mutex m_swapChainMutex;

	public:

		D3D11Interface();
		~D3D11Interface();

		void Initialize( HWND hwnd, bool fullscreen, unsigned int width, unsigned int height, bool vsyncEnabled, uint multiSampleCount = 1, uint multiSampleQuality = 0 );

		/*shuts down and releases all d3d interfaces*/
		void Shutdown();

		void DrawIndexed( unsigned int indexCount, unsigned int startIndex, int baseVertex );
		void DrawIndexedInstanced( unsigned int indexCountPerInstance, unsigned instanceCount, unsigned int indexStart, int baseVertexLocation, unsigned int instanceStart );

		void Present(bool vsyncEnabled);

		HRESULT MapResource( ID3D11Resource* buffer, unsigned int subResource, D3D11_MAP mapType, unsigned int flags, D3D11_MAPPED_SUBRESOURCE* resource );
		void UnmapResource( ID3D11Resource* resource, unsigned int subResource );

		//Copy the entire contents of the source resource to the destination resource using the GPU. 
		void CopyResource( ID3D11Resource* destination, ID3D11Resource* source );

		void UpdateSubresource( ID3D11Resource* resource, unsigned int subResource, const D3D11_BOX* destBox, const void* data, unsigned int srcRowPitch, unsigned int srcDepthPitch );

		/*sets the active render target and depth stencil*/
		void SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView);
		
		/*sets the back buffer to be the active render target*/
		void SetBackBufferRenderTarget();

		void SetDepthStencilState( ID3D11DepthStencilState* depthStencil );

		void ClearBackBuffer(float r, float g, float b, float a);
		void ClearDepthStencilView( ID3D11DepthStencilView* view, unsigned int clearFlags, float depth, unsigned char stencil );
		void ClearRenderTargetView( ID3D11RenderTargetView* rtView, const float* clearColor );

		HRESULT CreateBuffer( const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* srData, ID3D11Buffer** buffer );
		HRESULT CreateSampler( D3D11_SAMPLER_DESC* samplerDesc, ID3D11SamplerState** sampler );
		HRESULT CreateInputLayout( ID3D10Blob* vertexBlob, D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int layoutSize, ID3D11InputLayout** layout );

		void SetRasterState(ID3D11RasterizerState* state);

		/*disables blending*/
		void SetBlendState();

		/*sets the current blend state*/
		void SetBlendState(ID3D11BlendState* blendState, const Kiwi::Vector4d& blendFactor, UINT sampleMask);

		void SetViewports( D3D11_VIEWPORT viewports[], unsigned int numViewports );

		void SetVertexBuffers( unsigned int startSlot, unsigned int numBuffers, ID3D11Buffer** buffers, unsigned int* stride, unsigned int* offset );
		void SetIndexBuffer( ID3D11Buffer* buffer, DXGI_FORMAT format, unsigned int offset );

		void SetVertexShader( ID3D11VertexShader* vShader, ID3D11ClassInstance** classInstances, unsigned int numClassInstances );
		void SetPixelShader( ID3D11PixelShader* pShader, ID3D11ClassInstance** classInstances, unsigned int numClassInstances );
		void SetInputLayout( ID3D11InputLayout* layout );
		void SetVertexShaderCBuffers( unsigned int startSlot, unsigned int numBuffers, ID3D11Buffer** buffers );
		void SetPixelShaderCBuffers( unsigned int startSlot, unsigned int numBuffers, ID3D11Buffer** buffers );
		void SetShaderSamplers( unsigned int startSlot, unsigned int numSamplers, ID3D11SamplerState** samplers );
		void SetPixelShaderResources( unsigned int startSlot, unsigned int numViews, ID3D11ShaderResourceView** resources );

		/*unbinds all bound pixel shader resources*/
		void FreePixelShaderResources();

		/*unbinds a certain number of pixel shader resources starting from a certain slot
		>startSlot: the starting slot to being clearing from
		>numResources: the number of resources, including the start slot, to erase valid values are from 1 to D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - startSlot*/
		void FreePixelShaderResources( unsigned int startSlot, unsigned int numResources );
		

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