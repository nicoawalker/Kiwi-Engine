#ifndef _KIWI_RENDERER_H_
#define _KIWI_RENDERER_H_

#include "RenderTargetManager.h"
#include "Color.h"
#include "D3D11Interface.h"
#include "RenderStateObjectManager.h"
#include "RenderQueue.h"
#include "BlendState.h"
#include "GPUBufferManager.h"
#include "RenderQueue.h"
#include "RenderQueueGroup.h"

#include "..\Types.h"

#include "../Core/Vector2.h"
#include "..\Core\Vector4.h"
#include "../Core/IThreadSafe.h"
#include "..\Core\Math.h"

#include "../Events/IRenderEventEmitter.h"

#include <string>
#include <mutex>

namespace Kiwi
{

	struct BackBuffer;
	struct Submesh;

	class RenderWindow;
	class RenderTarget;
	class Viewport;
	class IShader;
	class IGPUBuffer;
	class Scene;
	class EngineRoot;

	struct MultisampleDesc
	{
		//The number of multisamples per pixel.
		unsigned int count;

		/*The image quality level. The higher the quality, the lower the performance.
		The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultisampleQualityLevels*/
		unsigned int quality;
	};

	struct RenderSettings
	{
		//the render target that is currently set to be drawn to
		Kiwi::RenderTarget* renderTarget;

		Kiwi::DepthStencil* depthStencil;

		Kiwi::PrimitiveTopology primitiveTopology;

		Kiwi::Viewport* viewport;

		Kiwi::RasterState* rasterState;

		Kiwi::BlendState* blendState;

		Kiwi::IShader* shader;

		Kiwi::Scene* activeScene;

		Kiwi::RenderQueue renderQueue;		

		RenderSettings()
		{
			renderTarget = nullptr;
			depthStencil = nullptr;
			viewport = nullptr;
			rasterState = nullptr;
			blendState = nullptr;
			shader = nullptr;
			activeScene = nullptr;
		}
	};

	struct D3DVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 bitangent;

		D3DVertex()
		{
			position = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
			tex = DirectX::XMFLOAT2( 0.0f, 0.0f );
			normal = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
			color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			tangent = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
			bitangent = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
		}

		D3DVertex( const Kiwi::Vector3d& pos, const Kiwi::Vector2d& textureUVs, const Kiwi::Vector3d& normals )
		{
			Kiwi::Vector3dToXMFLOAT3( pos, position );
			Kiwi::Vector2dToXMFLOAT2( textureUVs, tex );
			Kiwi::Vector3dToXMFLOAT3( normals, normal );
			color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			tangent = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
			bitangent = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
		}

		D3DVertex( const Kiwi::Vector3d& pos, const Kiwi::Vector2d& textureUVs, const Kiwi::Vector3d& normals, const int startWeight, const int weightCount )
		{
			Kiwi::Vector3dToXMFLOAT3( pos, position );
			Kiwi::Vector2dToXMFLOAT2( textureUVs, tex );
			Kiwi::Vector3dToXMFLOAT3( normals, normal );
			color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		}
	};

	class Renderer :
		public Kiwi::IThreadSafe,
		public Kiwi::IRenderEventEmitter
	{
	private:

		std::wstring m_name;

		Kiwi::D3D11Interface m_d3dInterface;

		Kiwi::RenderWindow* m_renderWindow;

		Kiwi::RenderTarget* m_backBuffer;

		Kiwi::RenderStateObjectManager m_rsoManager;

		/*manager used to create, store, find, and delete render targets*/
		Kiwi::RenderTargetManager m_renderTargetManager;

		Kiwi::GPUBufferManager m_gpuBufferManager;

		RenderSettings m_currentSettings;

		std::mutex m_renderer_M;

		bool m_depthEnable;

	protected:

		/*clears the currently active render target, including its depth buffer*/
		void _ClearRenderTarget( Kiwi::RenderTarget* rt );

		void _DrawRenderTarget( Kiwi::RenderTarget* rt );

		/*renders all of the active render target's render groups to that target*/
		void _RenderActiveTarget();

		/*renders a custom queue of meshes to the active render target*/
		void _RenderActiveTarget( Kiwi::RenderQueue& renderQueue );

		void _RenderMeshGroup( std::vector<Kiwi::Submesh*>& meshGroup, bool renderTransparency );

		/*renders a queue of models*/
		void _RenderModelQueue( RenderQueueGroup::ModelQueue& modelQueue );

	public:

		Renderer( std::wstring name, Kiwi::RenderWindow& window );
		~Renderer();

		Kiwi::BlendState* CreateBlendState( std::wstring name, const KiwiBlendDesc& rtBlendDesc, unsigned int sampleMask = 0xffffffff );

		/*Draws an indexed mesh, without instancing.
		indexCount: Number of indices to draw.
		startIndex: The location of the first index read by the GPU from the index buffer.
		baseVertex: A value added to each index before reading a vertex from the vertex buffer.
		*/
		void DrawIndexed( unsigned int indexCount, unsigned int startIndex, int baseVertex );

		/*Draws an indexed, instanced mesh. A Kiwi::InstancedMesh must be bound to be rendered
		indexCountPerInstance: Number of indices read from the index buffer for each instance.
		instanceCount: Number of instances to draw.
		indexStart: The location of the first index read by the GPU from the index buffer.
		baseVertexLocation: A value added to each index before reading a vertex from the vertex buffer.
		instanceStart: A value added to each index before reading per-instance data from a vertex buffer.*/
		void DrawIndexedInstanced( unsigned int indexCountPerInstance, unsigned instanceCount, unsigned int indexStart, int baseVertexLocation, unsigned int instanceStart );

		void Present();

		/*For each viewport in the render target, renders all renderqueuegroups that are assigned to it from renderQueue*/
		void Render();

		/*renders each of a render target's render groups to the render target*/
		void RenderToTexture( Kiwi::RenderTarget* renderTarget );

		/*renders each of a render target's render groups to the render target, if a target with matching name is found*/
		void RenderToTexture( const std::wstring& renderTarget );

		void RenderToTexture( Kiwi::RenderTarget* renderTarget, std::vector<Kiwi::Mesh*> meshesToRender );

		HRESULT MapResource( ID3D11Resource* resource, unsigned int subResource, D3D11_MAP mapType, unsigned int flags, D3D11_MAPPED_SUBRESOURCE* mappedResource );
		void UnmapResource( ID3D11Resource* resource, unsigned int subResource );

		void CopyResource( ID3D11Resource* destination, ID3D11Resource* source );

		void Update();

		void UpdateSubresource( ID3D11Resource* resource, unsigned int subResource, const D3D11_BOX* destBox, const void* data, unsigned int srcRowPitch, unsigned int srcDepthPitch );

		/*enables or disables the depth buffer for all future draw calls*/
		void EnableDepthBuffer( bool enable );

		/*clears the currently active render target, including its depth buffer*/
		void ClearActiveRenderTarget();

		/*clears the passed render target, including its depth buffer*/
		void ClearRenderTarget( Kiwi::RenderTarget* rt );

		void ClearRenderTarget( const std::wstring& renderTarget );

		Kiwi::RenderTarget* CreateRenderTarget( std::wstring name, Kiwi::Scene& scene, const Kiwi::Vector2d& dimensions );

		Kiwi::DepthStencil* CreateDepthStencil( std::wstring name, const DepthBufferProperties& bufferProperties, const DepthStencilProperties& stencilProperties );

		Kiwi::RasterState* CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc );

		ID3D11Buffer* CreateBuffer( const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* srData );

		ID3D11SamplerState* CreateSampler( D3D11_SAMPLER_DESC* samplerDesc );

		ID3D11InputLayout* CreateInputLayout( Kiwi::IShader* shader, D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int layoutSize );

		void DestroyRenderTarget( std::wstring renderTargetName ) { m_renderTargetManager.DestroyRenderTarget( renderTargetName ); }

		void SetPrimitiveTopology( Kiwi::PrimitiveTopology topology );

		/*binds a render target which will be used as the target for all future draw calls*/
		void SetRenderTarget( Kiwi::RenderTarget* target );

		/*sets the back buffer to be the active render target*/
		void SetBackBufferRenderTarget();

		/*sets the active depth stencil*/
		void SetDepthStencil( Kiwi::DepthStencil* depthStencil );

		void SetRasterState( Kiwi::RasterState* rasterState );
		void SetRasterState( std::wstring name );
		void SetRasterState( long uid );

		void SetBlendState( Kiwi::BlendState* blendState );

		void SetViewport( Kiwi::Viewport* viewport );

		void SetShader( Kiwi::IShader* shader );

		void SetActiveScene( Kiwi::Scene& scene );

		void SetPixelShaderResources( unsigned int startSlot, unsigned int numViews, ID3D11ShaderResourceView** resources );

		/*adds a group of meshes to the render queue for this frame
		this function should be called every frame with all desired meshes as the render queue is re-built each frame*/
		void SubmitMeshes( const std::vector<Kiwi::Mesh*>& meshes );

		void Shutdown();

		/*unbinds all bound pixel shader resources*/
		void FreePixelShaderResources() { m_d3dInterface.FreePixelShaderResources(); }

		/*unbinds a certain number of pixel shader resources starting from a certain slot
		>startSlot: the starting slot to being clearing from
		>numResources: the number of resources, including the start slot, to erase valid values are from 1 to D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - startSlot*/
		void FreePixelShaderResources( unsigned int startSlot, unsigned int numResources ) { m_d3dInterface.FreePixelShaderResources( startSlot, numResources ); }

		/*binds a single vertex buffer*/
		void SetVertexBuffer( unsigned int startSlot, Kiwi::IGPUBuffer* buffer, unsigned int offset );

		/*binds an array of vertex buffers*/
		void SetVertexBuffers( unsigned int startSlot, std::vector<Kiwi::IGPUBuffer*>& buffers, std::vector<unsigned int>& offsets );

		/*binds an index buffer*/
		void SetIndexBuffer( Kiwi::IGPUBuffer* buffer, DXGI_FORMAT format, unsigned int offset );

		void DestroyAllRasterStates();
		void DestroyRasterState( std::wstring name );

		std::wstring GetName()const { return m_name; }

		DXGI_FORMAT GetDXGIFormat()const;
		ID3D11Device* GetDevice()const;
		RenderSettings& GetRenderSettings() { return m_currentSettings; }

		Kiwi::RasterState* FindRasterState( std::wstring name );
		Kiwi::MultisampleDesc GetMultisampleDesc();
		Kiwi::RenderTarget* GetBackBuffer()const { return m_backBuffer; }
		Kiwi::RenderWindow* GetRenderWindow()const { return m_renderWindow; }
		Kiwi::RenderTarget* FindRenderTarget( std::wstring name ) { return m_renderTargetManager.FindRenderTarget( name ); }
		Kiwi::GPUBufferManager& GetGPUBufferManager() { return m_gpuBufferManager; }

	};
}

#endif