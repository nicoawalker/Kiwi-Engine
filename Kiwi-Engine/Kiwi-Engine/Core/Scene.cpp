#include "Scene.h"
#include "Utilities.h"
#include "Exception.h"
#include "Assert.h"
#include "EntityManager.h"
#include "AssetManager.h"
#include "SceneLoader.h"
#include "EngineRoot.h"

#include "../Graphics/Renderer.h"
#include "../Graphics/IShader.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/IRenderable.h"
#include "../Graphics/RenderTargetManager.h"

#include "../Graphics/D3D11Interface.h"
#include "../Graphics/DepthStencil.h"

namespace Kiwi
{

	Scene::Scene( Kiwi::EngineRoot* engine, std::wstring name, Kiwi::Renderer* renderer)
	{

		assert( engine != 0 );

		if(renderer == 0)
		{
			throw Kiwi::Exception(L"Scene", L"Invalid renderer");
		}

		m_sceneLoader = 0;

		try
		{
			m_name = name;

			m_renderer = renderer;

			m_diffuseLight = Kiwi::Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
			m_diffuseDirection = Kiwi::Vector4( 1.0f, -1.0f, 0.0f, 0.0f );
			m_ambientLight = Kiwi::Vector4( 0.4f, 0.4f, 0.4f, 1.0f );

			m_sceneLoader = new Kiwi::SceneLoader( this );

			//add the back buffer render target to the render target manager
			Kiwi::RenderTarget* backBuffer = m_renderer->GetBackBuffer();
			m_renderTargetManager.AddRenderTarget( backBuffer );

			m_engine = engine;

		} catch( const Kiwi::Exception& e )
		{
			throw e;
		}

	}

	Scene::~Scene()
	{

		m_renderer = 0;

		SAFE_DELETE( m_sceneLoader );

	}

	void Scene::Update()
	{

		m_entityManager.Update();

		this->Render();

	}

	void Scene::Load()
	{

		m_sceneLoader->QueueLoading<Kiwi::Mesh>( L"StaticMesh", L"TestMesh", L"H:\\Programming\\Projects\\Kiwi-Engine-Demo\\Kiwi Engine Demo\\Kiwi Engine Demo\\Data\\Models\\oldfarmhouse\\farmhouseobj.obj" );
		//TODO: Fix thread not ending bug in scene loader
		//m_sceneLoader->QueueLoading<Kiwi::Mesh>( L"StaticMesh", L"TestMesh2", L"H:\\Programming\\Projects\\Kiwi-Engine-Demo\\Kiwi Engine Demo\\Kiwi Engine Demo\\Data\\Models\\policeman\\policeman.obj" );
		//m_sceneLoader->QueueLoading<Kiwi::Mesh>( L"StaticMesh", L"TestMesh3", L"H:\\Programming\\Projects\\Kiwi-Engine-Demo\\Kiwi Engine Demo\\Kiwi Engine Demo\\Data\\Models\\policeman\\policeman.obj" );
		//m_sceneLoader->QueueLoading<Kiwi::Mesh>( L"StaticMesh", L"TestMesh4", L"H:\\Programming\\Projects\\Kiwi-Engine-Demo\\Kiwi Engine Demo\\Kiwi Engine Demo\\Data\\Models\\policeman\\policeman.obj" );

		m_sceneLoader->Start();
		while( !m_sceneLoader->Finished() )
		{

		}

		if( m_sceneLoader->GetExceptions().size() != 0 )
		{
			throw m_sceneLoader->GetExceptions()[0];
		}

	}

	void Scene::Render()
	{

		assert( m_renderer != 0 );

		try
		{

			bool renderTransparency = false; //if there are transparent entities this will be set to true

			bool once = true;

			//first get the 3D renderables
			Kiwi::RenderableManager::RenderableMap* renderables3D = m_renderableManager.Get3DRenderables();

			m_renderer->SetRasterState( L"Cull CCW" );

			//iterate over each render target
			auto renderTargetIt = renderables3D->rtMap.begin();
			for( ; renderTargetIt != renderables3D->rtMap.end(); renderTargetIt++ )
			{
				//get the render target
				Kiwi::RenderTarget* activeRenderTarget = this->FindRenderTargetWithName( renderTargetIt->first );
				if( activeRenderTarget != 0 )
				{
					//for each render target, if the render target exists, set it as active, clear it, set the viewport, and then get the shader map
					m_renderer->SetRenderTarget( activeRenderTarget );
					m_renderer->ClearRenderTarget();

					m_renderer->GetDeviceContext()->RSSetViewports( 1, &activeRenderTarget->GetViewport( 0 )->GetD3DViewport() );

					auto shaderIt = renderTargetIt->second.shaderMap.begin();
					for( ; shaderIt != renderTargetIt->second.shaderMap.end(); shaderIt++ )
					{
						//get the shader from the shader container
						Kiwi::IShader* shader = m_shaderContainer.Find( shaderIt->first );

						if( shader != 0 )
						{
							//for each shader, if the shader exists, set it as active and get the mesh map
							shader->Bind( m_renderer );

							shader->SetFrameParameters( this );

							auto meshItr = shaderIt->second.meshMap.begin();
							for( ; meshItr != shaderIt->second.meshMap.end(); meshItr++ )
							{
								//set the renderTransparency flag if there are any transparent renderables
								if( meshItr->second.transparent_renderables.size() > 0 ) renderTransparency = true;

								//TODO: z-sort the renderable list

								if( meshItr->second.opaque_renderables.size() > 0 )
								{
									//loop through all of the renderables using this mesh and render them
									auto renderableIt = meshItr->second.opaque_renderables.begin();

									//get a pointer to the mesh from the first renderable in the list
									Kiwi::Mesh* mesh = (*renderableIt)->GetMesh();

									if( mesh != 0 )
									{
										//bind the mesh's buffers
										mesh->Bind( m_renderer );

										//render each subset of the mesh
										for( unsigned int i = 0; i < mesh->GetSubsetCount(); i++ )
										{
										//render all of the renderables
										for( ; renderableIt != meshItr->second.opaque_renderables.end(); renderableIt++ )
										{
											
												//get the current subset
												Kiwi::Mesh::Subset* subset = mesh->GetSubset( i );
												//find the size of the subset
												unsigned int subsetSize = subset->endIndex - subset->startIndex;

												//set the renderable's current mesh subset so the shader knows what to render
												(*renderableIt)->SetCurrentMeshSubset( i );

												//set the renderable's shader parameters
												shader->SetObjectParameters( this, activeRenderTarget, *renderableIt );

												//now draw the renderable to the render target
												m_renderer->GetDeviceContext()->DrawIndexed( subsetSize, subset->startIndex, 0 );
											}
										}
									} else
									{
										MessageBox( NULL, L"No Mesh", L"A", MB_OK );
									}
								}
							}
						} else
						{
							MessageBox( NULL, L"No Shader", L"A", MB_OK );
						}
					}
				} else
				{
					MessageBox( NULL, L"No RenderTarget", L"A", MB_OK );
				}
			}

			//now get the 2D renderables
			//Kiwi::RenderableManager::RenderableMap* renderables2D = m_renderableManager.Get2DRenderables();

			m_renderer->Present();

		} catch( Kiwi::Exception& e )
		{
			MessageBox( NULL, e.GetError().c_str(), L"A", MB_OK );
		}

	}

	void Scene::AddEntity( Kiwi::Entity* entity )
	{

		if( entity == 0 ) return;

		try
		{
			m_entityManager.AddEntity( entity );

			if( entity->GetRenderable() != 0 )
			{
				m_renderableManager.AddRenderable( entity->GetRenderable() );
			}

		} catch( ... )
		{
			throw;
		}

	}

	void Scene::AddAsset( Kiwi::IAsset* asset )
	{

		try
		{
			m_assetManager.AddAsset( asset );

		} catch( ... )
		{
			throw;
		}

	}

	void Scene::AddShader( Kiwi::IShader* shader )
	{

		if( shader )
		{
			m_shaderContainer.Add( shader->GetName(), shader );
		}

	}

	Kiwi::Entity* Scene::FindEntityWithName( std::wstring name )
	{

		return m_entityManager.FindWithName( name );

	}

	std::vector<Kiwi::Entity*> Scene::FindEntitiesWithTag( std::wstring tag )
	{

		return m_entityManager.FindAllWithTag( tag );

	}

	Kiwi::RenderTarget* Scene::FindRenderTargetWithName( std::wstring name )
	{

		return m_renderTargetManager.FindRenderTargetWithName( name );

	}

};