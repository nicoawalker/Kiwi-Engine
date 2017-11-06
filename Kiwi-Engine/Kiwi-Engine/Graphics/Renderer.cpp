#include "Renderer.h"
#include "D3D11Interface.h"
#include "RenderWindow.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RasterStateManager.h"
#include "BlendState.h"
#include "Viewport.h"
#include "IShader.h"
#include "IGPUBuffer.h"
#include "Viewport.h"
#include "Camera.h"
#include "IModel.h"

#include "../Core/Exception.h"
#include "..\Core\Utilities.h"
#include "../Core/Assert.h"
#include "..\Core\Console.h"
#include "../Core/EngineRoot.h"

namespace Kiwi
{

	Renderer::Renderer( std::wstring name, Kiwi::RenderWindow& window ) :
		m_rsoManager( *this ), m_renderTargetManager( *this ), m_gpuBufferManager( *this )
	{
		m_backBuffer = 0;
		m_depthEnable = true;
		m_currentSettings.primitiveTopology = Kiwi::TRIANGLE_LIST;
		m_name = name;
		m_renderWindow = &window;

		try
		{
			//create the D3D interface
			m_d3dInterface.Initialize(window.GetHandle(), window.IsFullscreen(), (unsigned int)window.GetClientSize().x, (unsigned int)window.GetClientSize().y, window.GetVSyncEnabled());
			
			//create a default rendertarget for the back buffer
			Kiwi::BackBuffer* bb = m_d3dInterface.GetBackBuffer();
			m_backBuffer = m_renderTargetManager.CreateRenderTarget( L"Backbuffer", 0, bb->texture, 0, bb->renderTargetView );

			//create the back buffer's depth stencil
			Kiwi::DepthStencil* bbDepth = m_rsoManager.CreateDepthStencil( L"Backbuffer/DepthStencil", bb->depthBuffer, bb->depthStencilView, bb->depthStencilState, bb->depthDisabledState );
			m_backBuffer->AttachDepthStencil(bbDepth);

			//create a default viewport for the back buffer
			m_backBuffer->CreateViewport( L"Backbuffer/Viewport", 0 );

			m_currentSettings.renderTarget = m_backBuffer;

			/*create raster state to enable drawing in wireframe*/
			Kiwi::D3DRasterStateDesc wireframeDesc;
			wireframeDesc.desc.CullMode = D3D11_CULL_NONE;
			wireframeDesc.desc.FillMode = D3D11_FILL_WIREFRAME;
			this->CreateRasterState( L"Wireframe", wireframeDesc );

			/*create rasterstates for clockwise and counterclockwise culling (for transparency)*/
			Kiwi::D3DRasterStateDesc cDesc;
			cDesc.desc.FillMode = D3D11_FILL_SOLID;
			cDesc.desc.CullMode = D3D11_CULL_BACK;
			cDesc.desc.FrontCounterClockwise = true;
			this->CreateRasterState( L"CullCW", cDesc );
			cDesc.desc.FrontCounterClockwise = false;
			this->CreateRasterState( L"CullCCW", cDesc );
		
		}catch(Kiwi::Exception& e)
		{
			KIWIDEBUGSTRING( Kiwi::ToWString( e.GetError() + L"\n" ).c_str() );
			throw;
		}
	}

	Renderer::~Renderer()
	{
	}

	/*clears the passed render target as well as its depth stencil*/
	void Renderer::_ClearRenderTarget( Kiwi::RenderTarget* rt )
	{
		if( rt == nullptr ) return;

		Kiwi::Color color = rt->GetClearColor();
		float col[4] = { (float)color.red, (float)color.green, (float)color.blue, (float)color.alpha };

		m_d3dInterface.ClearRenderTargetView( rt->GetRenderTargetView(), col );

		Kiwi::DepthStencil* stencil = rt->GetDepthStencil();
		if( stencil != 0 && stencil->GetD3DView() != 0 )
		{
			m_d3dInterface.ClearDepthStencilView( stencil->GetD3DView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
		}
	}

	void Renderer::_RenderModelQueue( RenderQueueGroup::ModelQueue& modelQueue )
	{
		if( m_currentSettings.activeScene == nullptr ) return;

		long currentMaterialUID = -1;
		Kiwi::IModel* lastModel = nullptr;
		Kiwi::IModel::Subset* lastSubset = nullptr;
		unsigned long meshID = 0;

		for( auto modelItr = modelQueue.begin(); modelItr != modelQueue.end(); modelItr++ )
		{
			Kiwi::IModel* model = (*modelItr).model;
			Kiwi::IModel::Subset* subset = (*modelItr).subset;
			Kiwi::Material* material = subset->GetMaterial();

			if( model == nullptr || subset == nullptr || material == nullptr ) continue;

			/*if the shader has changed, switch to the new shader and set the shader's frame parameters*/
			if( m_currentSettings.shader == nullptr || m_currentSettings.shader->GetUID() != material->GetShaderID() )
			{
				this->SetShader( material->GetShader() );

				if( m_currentSettings.shader == nullptr ) continue;

				m_currentSettings.shader->SetPerFrame( *m_currentSettings.activeScene );
			}

			/*if the material has changed, update the shader's per-material buffers*/
			if( material->GetUID() != currentMaterialUID )
			{
				currentMaterialUID = material->GetUID();
				m_currentSettings.shader->SetPerMaterial( *m_currentSettings.activeScene, *material );
			}

			/*if the model has changed*/
			if( model != lastModel || subset != lastSubset )
			{
				lastModel = model;
				lastSubset = subset;

				/*attempt to bind the subset's gpu buffers*/
				if( !model->Bind( *this, modelItr->subsetIndex ) )
				{
					continue;
				}

				m_currentSettings.shader->SetPerObject( *m_currentSettings.activeScene, *m_currentSettings.viewport, *model, modelItr->subsetIndex );

				/*change the primitive topology if required*/
				if( model->GetPrimitiveTopology() != m_currentSettings.primitiveTopology )
				{
					this->SetPrimitiveTopology( model->GetPrimitiveTopology() );
				}
			}

			if( model->IsShared() )
			{
				/*shared models are rendered using instancing*/
				if( material->HasTransparency() )
				{
					/*draw back faces before front faces to render transparency correctly*/
					this->SetRasterState( L"CullCW" );
					this->DrawIndexedInstanced( subset->numIndices, (unsigned int)model->GetInstanceData().size(), subset->indexOffset, subset->vertexOffset, 0 );
				}

				/*draw front faces*/
				this->SetRasterState( L"CullCCW" );
				this->DrawIndexedInstanced( subset->numIndices, (unsigned int)model->GetInstanceData().size(), subset->indexOffset, subset->vertexOffset, 0 );

			} else
			{
				/*rendering the model normally, without instancing*/
				Kiwi::Entity* parent = (*(model->GetLinkedMeshes().begin()))->GetEntity();
				if( parent == nullptr )continue;

				if( material->HasTransparency() && parent->GetType() != Kiwi::Entity::ENTITY_2D )
				{
					/*draw back faces before front faces to render transparency correctly*/
					this->SetRasterState( L"CullCW" );
					this->DrawIndexed( subset->numIndices, subset->indexOffset, subset->vertexOffset );
				}

				/*draw front faces*/
				this->SetRasterState( L"CullCCW" );
				this->DrawIndexed( subset->numIndices, subset->indexOffset, subset->vertexOffset );
			}
		}

		this->FreePixelShaderResources();
	}

	Kiwi::BlendState* Renderer::CreateBlendState( std::wstring name, const KiwiBlendDesc& rtBlendDesc, unsigned int sampleMask )
	{
		return m_rsoManager.CreateBlendState( name, rtBlendDesc, sampleMask );
	}

	void Renderer::DrawIndexed( unsigned int indexCount, unsigned int startIndex, int baseVertex )
	{
		m_d3dInterface.DrawIndexed( indexCount, startIndex, baseVertex );
	}

	void Renderer::DrawIndexedInstanced( unsigned int indexCountPerInstance, unsigned instanceCount, unsigned int indexStart, int baseVertexLocation, unsigned int instanceStart )
	{
		m_d3dInterface.DrawIndexedInstanced( indexCountPerInstance, instanceCount, indexStart, baseVertexLocation, instanceStart );
	}

	void Renderer::Present()
	{
		if( m_renderWindow != 0 )
		{
			m_d3dInterface.Present( m_renderWindow->GetVSyncEnabled() );
		}
	}

	void Renderer::_DrawRenderTarget( Kiwi::RenderTarget* rt )
	{
		if( rt->GetViewportCount() == 0 ) return;

		Kiwi::RenderQueue* rq = &rt->GetRenderQueue();

		long long shaderID = 0;
		long long viewportIndex = 0;
		long long translucencyType = 0;
		long long materialID = 0;
		long long is2D = 0;
		Kiwi::IModel::Subset* subset = nullptr;
		Kiwi::IModel* model = nullptr;
		Kiwi::Material* material = nullptr;

		this->SetViewport( rt->GetViewport( 0 ) );

		for( auto renderableItr = rq->Begin(); renderableItr != rq->End(); renderableItr++ )
		{
			//skip duplicate subsets due to shared models being added multiple times
			if( renderableItr->model->IsShared() && 
				renderableItr->model == model && 
				renderableItr->model->GetSubset( renderableItr->subsetIndex ) == subset ) continue;

			subset = renderableItr->model->GetSubset( renderableItr->subsetIndex );

			//test viewport bits
			long long renderKey = (renderableItr->renderKey & Kiwi::RenderableBitmask::ViewportIndex) >> Kiwi::RenderableBitmask::Bit::ViewportIndex;
			if( renderKey != viewportIndex )
			{
				/*changing viewport*/
				viewportIndex = renderKey;

				if( viewportIndex >= rt->GetViewportCount() ) viewportIndex = 0;

				this->SetViewport( rt->GetViewport( (unsigned int)viewportIndex ) );
			}

			//test translucency bits
			renderKey = (renderableItr->renderKey & Kiwi::RenderableBitmask::TranslucencyType) >> Kiwi::RenderableBitmask::Bit::TranslucencyType;
			if( renderKey != translucencyType )
			{
				/*changing translucency type*/
				translucencyType = renderKey;
				if( translucencyType == 0 )
				{
					//opaque
				} else
				{
					//non-opaque
				}
			}

			//test material bits
			renderKey = (renderableItr->renderKey & Kiwi::RenderableBitmask::Material) >> Kiwi::RenderableBitmask::Bit::Material;
			if( renderKey != materialID )
			{
				/*changing material*/
				materialID = renderKey;
				material = subset->GetMaterial();

				/*test shader bits*/
				renderKey = (renderableItr->renderKey & Kiwi::RenderableBitmask::Shader) >> Kiwi::RenderableBitmask::Bit::Shader;
				if( renderKey != shaderID )
				{
					/*need to change shaders*/
					shaderID = renderKey;

					this->SetShader( material->GetShader() );

					if( m_currentSettings.shader == nullptr ) continue;
					
					m_currentSettings.shader->SetPerFrame( *m_currentSettings.activeScene );
				}

				if( m_currentSettings.shader != nullptr ) m_currentSettings.shader->SetPerMaterial( *m_currentSettings.activeScene, *material );
			}

			if( m_currentSettings.shader == nullptr ) continue;

			if( renderableItr->model != model )
			{
				model = renderableItr->model;

				/*attempt to bind the subset's gpu buffers*/
				if( !model->Bind( *this, renderableItr->subsetIndex ) )
				{
					continue;
				}

				/*change the primitive topology if required*/
				if( model->GetPrimitiveTopology() != m_currentSettings.primitiveTopology )
				{
					this->SetPrimitiveTopology( model->GetPrimitiveTopology() );
				}
			}

			m_currentSettings.shader->SetPerObject( *m_currentSettings.activeScene, *m_currentSettings.viewport, *model, renderableItr->subsetIndex );

			renderKey = (renderableItr->renderKey & Kiwi::RenderableBitmask::Is2D) >> Kiwi::RenderableBitmask::Bit::Is2D;
			if( renderKey != is2D )
			{
				is2D = renderKey;

				if( is2D == 1 )
				{
					//rendering 2d models, disable depth buffer
					this->EnableDepthBuffer( false );

				} else
				{
					this->EnableDepthBuffer( true );
				}
			}

			if( model->IsShared() )
			{
				/*shared models are rendered using instancing*/
				if( translucencyType != 0 )
				{
					/*draw back faces before front faces to render transparency correctly*/
					this->SetRasterState( L"CullCW" );
					this->DrawIndexedInstanced( subset->numIndices, (unsigned int)model->GetInstanceData().size(), subset->indexOffset, subset->vertexOffset, 0 );
				}

				/*draw front faces*/
				this->SetRasterState( L"CullCCW" );
				this->DrawIndexedInstanced( subset->numIndices, (unsigned int)model->GetInstanceData().size(), subset->indexOffset, subset->vertexOffset, 0 );

			} else
			{
				if( translucencyType != 0 && is2D == 0 )
				{
					/*draw back faces before front faces to render transparency correctly*/
					this->SetRasterState( L"CullCW" );
					this->DrawIndexed( subset->numIndices, subset->indexOffset, subset->vertexOffset );
				}

				/*draw front faces*/
				this->SetRasterState( L"CullCCW" );
				this->DrawIndexed( subset->numIndices, subset->indexOffset, subset->vertexOffset );
			}
		}

		this->FreePixelShaderResources();
	}

	void Renderer::Render()
	{
		std::unique_lock<std::mutex> ulock( m_renderer_M );

		/*let listeners know that rendering is about to begin*/
		this->EmitRenderEvent( std::make_shared<Kiwi::RenderEvent>( this, L"PreRender" ) );

		Kiwi::RenderTargetList rtList = m_renderTargetManager.GetRenderTargetList();
		for( auto itr = rtList.targets.begin(); itr != rtList.targets.end(); itr++ )
		{
			if( (*itr)->AutoRenderEnabled() == false ) continue; //dont auto-render render targets  set to manual
			if( (*itr)->GetName().compare( L"Backbuffer" ) == 0 ) continue; //skip rendering to the back buffer for now

			this->SetRenderTarget( *itr );
			this->_ClearRenderTarget( *itr );

			(*itr)->OnPreRender();

			this->_DrawRenderTarget( *itr );

			(*itr)->OnPostRender();
		}

		this->_ClearRenderTarget( m_backBuffer );
		this->SetRenderTarget( m_backBuffer );

		m_backBuffer->OnPreRender();

		this->_DrawRenderTarget( m_backBuffer );

		m_backBuffer->OnPostRender();

		/*let listeners know that rendering has ended*/
		this->EmitRenderEvent( std::make_shared<Kiwi::RenderEvent>( this, L"PostRender" ) );
	}

	void Renderer::RenderToTexture( Kiwi::RenderTarget* renderTexture )
	{
		if( renderTexture == nullptr ) return;

		std::unique_lock<std::mutex> ulock( m_renderer_M );

		if( m_currentSettings.renderTarget != renderTexture ) this->SetRenderTarget( renderTexture );
		//this->_RenderActiveTarget();
	}

	void Renderer::RenderToTexture( const std::wstring& renderTexture )
	{
		std::unique_lock<std::mutex> ulock( m_renderer_M );

		Kiwi::RenderTarget* renderTarget = m_renderTargetManager.FindRenderTarget( renderTexture );
		if( renderTarget == nullptr ) return;

		if( m_currentSettings.renderTarget != renderTarget ) this->SetRenderTarget( renderTarget );
		//this->_RenderActiveTarget();
	}

	void Renderer::RenderToTexture( Kiwi::RenderTarget* renderTexture, std::vector<Kiwi::Mesh*> meshesToRender )
	{
		if( renderTexture == nullptr ) return;

		std::unique_lock<std::mutex> ulock( m_renderer_M );

		/*create a temporary render queue out of the mesh group*/
		RenderQueue tempQueue;
		tempQueue.AddMeshes( meshesToRender );

		/*set the active render target and render the temporary render queue to it*/
		if( m_currentSettings.renderTarget != renderTexture ) this->SetRenderTarget( renderTexture );
		//this->_RenderActiveTarget(tempQueue);
	}

	HRESULT Renderer::MapResource( ID3D11Resource* resource, unsigned int subResource, D3D11_MAP mapType, unsigned int flags, D3D11_MAPPED_SUBRESOURCE* mappedResource )
	{
		return m_d3dInterface.MapResource( resource, subResource, mapType, flags, mappedResource );
	}

	void Renderer::UnmapResource( ID3D11Resource* resource, unsigned int subResource )
	{
		m_d3dInterface.UnmapResource( resource, subResource );
	}

	void Renderer::CopyResource( ID3D11Resource* destination, ID3D11Resource* source )
	{
		m_d3dInterface.CopyResource( destination, source );
	}

	void Renderer::UpdateSubresource( ID3D11Resource* resource, unsigned int subResource, const D3D11_BOX* destBox, const void* data, unsigned int srcRowPitch, unsigned int srcDepthPitch )
	{
		m_d3dInterface.UpdateSubresource( resource, subResource, destBox, data, srcRowPitch, srcDepthPitch );
	}

	Kiwi::RenderTarget* Renderer::CreateRenderTarget( std::wstring name, Kiwi::Scene& scene, const Kiwi::Vector2d& dimensions )
	{
		Kiwi::RenderTarget* newTarget = m_renderTargetManager.CreateRenderTarget( name, &scene, dimensions );

		if( newTarget != 0 )
		{
			/*create a new default depth stencil and attach it*/
			Kiwi::DepthStencil* stencil = this->CreateDepthStencil( name + L"/DepthStencil", Kiwi::DepthBufferProperties(), Kiwi::DepthStencilProperties() );
			newTarget->AttachDepthStencil( stencil );
		}

		return newTarget;
	}

	Kiwi::DepthStencil* Renderer::CreateDepthStencil( std::wstring name, const DepthBufferProperties& bufferProperties, const DepthStencilProperties& stencilProperties )
	{
		return m_rsoManager.CreateDepthStencil( name, bufferProperties, stencilProperties );
	}

	void Renderer::EnableDepthBuffer( bool enable )
	{
		if( enable != m_depthEnable )
		{
			m_depthEnable = enable;

			this->SetDepthStencil( m_currentSettings.depthStencil );
		}
	}

	void Renderer::ClearActiveRenderTarget()
	{
		std::lock_guard<std::mutex> lock( m_renderer_M );

		this->_ClearRenderTarget( m_currentSettings.renderTarget );
	}

	/*clears the passed render target as well as its depth stencil*/
	void Renderer::ClearRenderTarget( Kiwi::RenderTarget* rt )
	{
		std::lock_guard<std::mutex> lock( m_renderer_M );

		this->_ClearRenderTarget( rt );
	}

	void Renderer::ClearRenderTarget( const std::wstring& renderTarget )
	{
		std::lock_guard<std::mutex> lock( m_renderer_M );

		Kiwi::RenderTarget* rt = m_renderTargetManager.FindRenderTarget( renderTarget );
		if( rt == nullptr ) return;

		this->_ClearRenderTarget( rt );
	}

	Kiwi::RasterState* Renderer::CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc )
	{
		return m_rsoManager.CreateRasterState( name, desc );
	}

	ID3D11Buffer* Renderer::CreateBuffer( const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* srData )
	{
		std::lock_guard<std::mutex> lock( m_renderer_M );

		if( bufferDesc )
		{
			ID3D11Buffer* buffer = 0;

			HRESULT hr = m_d3dInterface.CreateBuffer( bufferDesc, srData, &buffer );
			if( FAILED(hr) )
			{
				throw Kiwi::Exception( L"Renderer::CreateBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ), KIWI_EXCEPTION::OBJECTCREATIONFAILED );
			}

			return buffer;
		}

		return 0;
	}

	ID3D11SamplerState* Renderer::CreateSampler( D3D11_SAMPLER_DESC* samplerDesc )
	{
		std::lock_guard<std::mutex> lock( m_renderer_M );

		if( samplerDesc )
		{
			ID3D11SamplerState* sampler = 0;

			HRESULT hr = m_d3dInterface.CreateSampler( samplerDesc, &sampler );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"Renderer::CreateSampler", L"Failed to create sampler: " + Kiwi::GetD3DErrorString( hr ), KIWI_EXCEPTION::OBJECTCREATIONFAILED );
			}

			return sampler;
		}

		return 0;
	}

	ID3D11InputLayout* Renderer::CreateInputLayout( Kiwi::IShader* shader, D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int layoutSize )
	{
		std::lock_guard<std::mutex> lock( m_renderer_M );

		if( shader && shader->m_vertexBlob && polygonLayout )
		{
			ID3D11InputLayout* layout = 0;
			HRESULT hr = m_d3dInterface.CreateInputLayout( shader->m_vertexBlob, polygonLayout, layoutSize, &layout );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"Renderer::CreateInputLayout", L"Failed to create input layout: " + Kiwi::GetD3DErrorString( hr ), KIWI_EXCEPTION::OBJECTCREATIONFAILED );
			}

			return layout;
		}

		return 0;
	}

	void Renderer::SetPrimitiveTopology( Kiwi::PrimitiveTopology topology )
	{
		if( topology != m_currentSettings.primitiveTopology )
		{
			try
			{
				m_d3dInterface.SetPrimitiveTopology( (D3D_PRIMITIVE_TOPOLOGY)topology );
				m_currentSettings.primitiveTopology = topology;

			} catch( Kiwi::Exception& e )
			{
				KIWIDEBUGSTRING( Kiwi::ToWString(e.GetError() + L"\n").c_str() );
			}
		}
	}

	void Renderer::SetRenderTarget( Kiwi::RenderTarget* renderTarget )
	{
		if( renderTarget )
		{
			Kiwi::DepthStencil* depthState = renderTarget->GetDepthStencil();
			Kiwi::BlendState* blendState = renderTarget->GetBlendState();
			unsigned int viewportCount = renderTarget->GetViewportCount();
			D3D11_VIEWPORT* viewports = new D3D11_VIEWPORT[viewportCount];

			/*if the render target doesn't have a blend state set yet, try to find a matching one and set it here*/
			if( blendState == nullptr )
			{
				Kiwi::BlendState* blendState = m_rsoManager.FindBlendState( renderTarget->GetBlendStateName() );
				if( blendState == nullptr )
				{
					return;

				} else
				{
					renderTarget->SetBlendState( blendState );
				}
			}

			m_d3dInterface.SetRenderTarget( renderTarget->GetRenderTargetView(), NULL );			

			this->SetDepthStencil( depthState );

			if( depthState != 0 )
			{
				m_d3dInterface.SetRenderTarget( renderTarget->GetRenderTargetView(), depthState->GetD3DView() );

			} else
			{
				m_d3dInterface.SetRenderTarget( renderTarget->GetRenderTargetView(), NULL );
			}

			this->SetBlendState( blendState );

			m_currentSettings.renderTarget = renderTarget;

			SAFE_DELETE( viewports );
		}
	}

	void Renderer::SetBackBufferRenderTarget()
	{
		this->SetRenderTarget( m_backBuffer );
	}

	void Renderer::SetDepthStencil( Kiwi::DepthStencil* depthStencil )
	{
		if( depthStencil )
		{
			if( m_depthEnable )
			{
				m_d3dInterface.SetDepthStencilState( depthStencil->GetD3DState() );

			} else
			{
				m_d3dInterface.SetDepthStencilState( depthStencil->GetD3DDisabledState() );
			}

		} else
		{
			m_d3dInterface.SetDepthStencilState( 0 );
		}

		m_currentSettings.depthStencil = depthStencil;
	}

	void Renderer::SetRasterState( Kiwi::RasterState* rasterState )
	{
		if( rasterState && rasterState != m_currentSettings.rasterState )
		{
			m_d3dInterface.SetRasterState( rasterState->GetD3DRasterState() );
			m_currentSettings.rasterState = rasterState;
		}
	}

	void Renderer::SetRasterState( std::wstring name )
	{
		this->SetRasterState( m_rsoManager.FindRasterState( name ) );
	}

	void Renderer::SetRasterState( long uid )
	{
		this->SetRasterState( m_rsoManager.FindRasterState( uid ) );
	}

	void Renderer::SetBlendState( Kiwi::BlendState* blendState )
	{
		if( blendState != m_currentSettings.blendState )
		{
			if( blendState != nullptr )
			{
				m_d3dInterface.SetBlendState( blendState->GetD3DBlendState(), m_currentSettings.renderTarget->GetBlendingFactor(), blendState->GetSampleMask() );

			} else
			{
				//turn off blending if 0 is passed
				m_d3dInterface.SetBlendState();
			}
			m_currentSettings.blendState = blendState;
		}
	}

	void Renderer::SetViewport( Kiwi::Viewport* viewport )
	{
		if( viewport )
		{
			D3D11_VIEWPORT viewports[1] = { viewport->GetD3DViewport() };

			m_d3dInterface.SetViewports( viewports, 1 );

			/*build the viewport's orthographic matrix*/
			if(viewport->GetCamera()) viewport->GetCamera()->SetViewport( viewport );

			m_currentSettings.viewport = viewport;
		}
	}

	void Renderer::SetShader( Kiwi::IShader* shader )
	{
		if( shader != m_currentSettings.shader )
		{
			if( shader )
			{
				m_d3dInterface.SetInputLayout( shader->m_inputLayout );
				m_d3dInterface.SetVertexShader( shader->m_vertexShader, 0, 0 );
				m_d3dInterface.SetPixelShader( shader->m_pixelShader, 0, 0 );

				if( shader->m_vBuffers.size() > 0 )
				{
					m_d3dInterface.SetVertexShaderCBuffers( 0, (unsigned int)shader->m_vBuffers.size(), &shader->m_vBuffers[0] );
				}
				if( shader->m_pBuffers.size() > 0 )
				{
					m_d3dInterface.SetPixelShaderCBuffers( 0, (unsigned int)shader->m_pBuffers.size(), &shader->m_pBuffers[0] );
				}
				if( shader->m_samplerStates.size() > 0 )
				{
					m_d3dInterface.SetShaderSamplers( 0, (unsigned int)shader->m_samplerStates.size(), &shader->m_samplerStates[0] );
				}

			} else
			{
				//unbind shaders
				m_d3dInterface.SetVertexShader( 0, 0, 0 );
				m_d3dInterface.SetPixelShader( 0, 0, 0 );
			}

			m_currentSettings.shader = shader;
		}
	}

	void Renderer::SetActiveScene( Kiwi::Scene& scene )
	{
		m_currentSettings.activeScene = &scene;
	}

	void Renderer::SetPixelShaderResources( unsigned int startSlot, unsigned int numViews, ID3D11ShaderResourceView** resources )
	{
		if( resources )
		{
			m_d3dInterface.SetPixelShaderResources( startSlot, numViews, resources );
		}
	}

	void Renderer::SubmitMeshes( const std::vector<Kiwi::Mesh*>& meshes )
	{
		m_currentSettings.renderQueue.AddMeshes( meshes );
	}

	/*binds a single vertex buffer*/
	void Renderer::SetVertexBuffer( unsigned int startSlot, Kiwi::IGPUBuffer* buffer, unsigned int offset )
	{
		if( buffer )
		{
			ID3D11Buffer* buf = buffer->_GetD3D();
			if( buf != nullptr )
			{
				unsigned int stride = buffer->GetStride();
				m_d3dInterface.SetVertexBuffers( startSlot, 1, &buf, &stride, &offset );
			}
		}
	}

	/*binds an array of vertex buffers*/
	void Renderer::SetVertexBuffers( unsigned int startSlot, std::vector<Kiwi::IGPUBuffer*>& buffers, std::vector<unsigned int>& offsets )
	{
		if( buffers.size() > 0 )
		{
			ID3D11Buffer** buf = new ID3D11Buffer*[buffers.size()];
			unsigned int* strides = new unsigned int[buffers.size()];
			for( unsigned int i = 0; i < buffers.size(); i++ )
			{
				buf[i] = buffers[i]->_GetD3D();
				strides[i] = buffers[i]->GetStride();
			}
			m_d3dInterface.SetVertexBuffers( startSlot, (unsigned int)buffers.size(), buf, &strides[0], &offsets[0] );
		}
	}

	/*binds an index buffer*/
	void Renderer::SetIndexBuffer( Kiwi::IGPUBuffer* buffer, DXGI_FORMAT format, unsigned int offset )
	{
		if( buffer )
		{
			m_d3dInterface.SetIndexBuffer( buffer->_GetD3D(), format, offset );
		}
	}

	void Renderer::Shutdown()
	{
		m_rsoManager.Shutdown();
		m_gpuBufferManager.Shutdown();
		m_renderTargetManager.Shutdown();
		m_currentSettings.activeScene = nullptr;
		m_currentSettings.blendState = nullptr;
		m_currentSettings.depthStencil = nullptr;
		m_currentSettings.rasterState = nullptr;
		m_currentSettings.renderTarget = nullptr;
		m_currentSettings.shader = nullptr;
		m_currentSettings.viewport = nullptr;
	}

	void Renderer::Update()
	{
		/*clear the render queue so that it can be rebuilt each frame*/
		m_currentSettings.renderQueue.Clear();
	}

	void Renderer::DestroyAllRasterStates()
	{
		m_rsoManager.DestroyAllRasterStates();
	}

	void Renderer::DestroyRasterState( std::wstring name )
	{
		m_rsoManager.DestroyRasterState( name );
	}

	DXGI_FORMAT Renderer::GetDXGIFormat()const
	{
		return m_d3dInterface.GetDXGIFormat();
	}

	Kiwi::MultisampleDesc Renderer::GetMultisampleDesc()
	{
		return Kiwi::MultisampleDesc{ m_d3dInterface.GetMultisampleCount(), m_d3dInterface.GetMultisampleQuality() };
	}

	ID3D11Device* Renderer::GetDevice()const
	{
		return m_d3dInterface.GetDevice();
	}

	Kiwi::RasterState* Renderer::FindRasterState( std::wstring name )
	{
		return m_rsoManager.FindRasterState( name );
	}

};