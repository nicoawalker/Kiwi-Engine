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
#include "../Graphics/InstancedMesh.h"
#include "../Graphics/IRenderable.h"
#include "../Graphics/RenderTargetManager.h"

#include "../Graphics/D3D11Interface.h"
#include "../Graphics/DepthStencil.h"

namespace Kiwi
{

	Scene::Scene( Kiwi::EngineRoot* engine, std::wstring name, Kiwi::Renderer* renderer )
	{

		assert( engine != 0 );

		if( renderer == 0 )
		{
			throw Kiwi::Exception( L"Scene", L"Invalid renderer" );
		}

		m_sceneLoader = 0;
		m_playerEntity = 0;

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

	void Scene::_Update()
	{

		if( m_playerEntity ) m_playerEntity->Update();

		m_entityManager.Update();

		this->OnUpdate();

		this->_Render();

	}

	void Scene::_FixedUpdate()
	{

		if( m_playerEntity ) m_playerEntity->FixedUpdate();

		m_entityManager.FixedUpdate();

		this->OnFixedUpdate();

	}

	void Scene::_Render()
	{

		assert( m_renderer != 0 );

		this->OnPreRender();

		try
		{
			//get the renderables
			Kiwi::RenderableManager::RenderableMap* renderables = m_renderableManager.GetRenderables();

			//iterate over each render target
			auto renderTargetIt = renderables->rtMap.begin();
			for( ; renderTargetIt != renderables->rtMap.end(); renderTargetIt++ )
			{
				//get the render target
				Kiwi::RenderTarget* activeRenderTarget = this->FindRenderTargetWithName( renderTargetIt->first );
				if( activeRenderTarget != 0 )
				{
					//for each render target, if the render target exists, set it as active, clear it, set the viewport, and then get the shader map
					m_renderer->SetRenderTarget( activeRenderTarget );
					m_renderer->ClearRenderTarget();

					m_renderer->GetDeviceContext()->RSSetViewports( 1, &activeRenderTarget->GetViewport( 0 )->GetD3DViewport() );

					//set the raster state to cull back-faces when rendering the opaque renderables
					m_renderer->SetRasterState( L"Cull CCW" );

					//first render all of the 3D renderables
					this->_Render3D( renderTargetIt->second );

					//then render all of the 2D renderables
					this->_Render2D( renderTargetIt->second );

				} else
				{
					MessageBox( NULL, L"No RenderTarget", L"A", MB_OK );
				}
			}

			m_renderer->Present();

		} catch( Kiwi::Exception& e )
		{
			MessageBox( NULL, e.GetError().c_str(), L"A", MB_OK );
		}

		this->OnPostRender();

	}

	void Scene::_Render3D(RenderableManager::RenderableShaderMap& shaderMap)
	{

		//if there are transparent entities this will be set to true so we know to render
		//the transparent renderables after all of the others
		bool renderTransparency = false;

		//first render all of the 3D renderables
		auto shaderIt = shaderMap.shaderMap.begin();
		for( ; shaderIt != shaderMap.shaderMap.end(); shaderIt++ )
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
					if( meshItr->second.transparent_renderables.size() > 0 )
					{
						renderTransparency = true;
					}

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
							try
							{
								//bind will throw if the mesh buffers have not been initialized
								mesh->Bind( m_renderer );

							} catch( Kiwi::Exception& e )
							{
								//cant render this mesh, move on to the next one
								continue;
							}

							//render each subset of the mesh
							for( unsigned int i = 0; i < mesh->GetSubsetCount(); i++ )
							{
								//render this subset for all of the renderables
								for( ; renderableIt != meshItr->second.opaque_renderables.end(); renderableIt++ )
								{
									if( (*renderableIt)->GetParentEntity() && (*renderableIt)->GetParentEntity()->IsActive() == false )
									{
										//don't render any renderables that belong to an inactive entity
										continue;
									}

									//get the current subset
									Kiwi::Mesh::Subset* subset = mesh->GetSubset( i );
									//find the size of the subset
									unsigned int subsetSize = subset->endIndex - subset->startIndex;

									//set the renderable's current mesh subset so the shader knows what to render
									(*renderableIt)->SetCurrentMeshSubset( i );

									//set the renderable's shader parameters
									shader->SetObjectParameters( this, m_renderer->GetActiveRenderTarget(), *renderableIt );

									if( mesh->IsInstanced() )
									{
										//draw all of the instances of the mesh
										m_renderer->GetDeviceContext()->DrawIndexedInstanced( subsetSize, mesh->GetInstanceCount(), subset->startIndex, 0, 0 );

									} else
									{
										//render the mesh without instancing
										m_renderer->GetDeviceContext()->DrawIndexed( subsetSize, subset->startIndex, 0 );
									}
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

		//if there were transparent 3D renderables we skipped, render them now at the end
		if( renderTransparency )
		{
			shaderIt = shaderMap.shaderMap.begin();
			for( ; shaderIt != shaderMap.shaderMap.end(); shaderIt++ )
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
						//TODO: z-sort the renderable list

						if( meshItr->second.transparent_renderables.size() > 0 )
						{
							//loop through all of the renderables using this mesh and render them
							auto renderableIt = meshItr->second.transparent_renderables.begin();

							//get a pointer to the mesh from the first renderable in the list
							Kiwi::Mesh* mesh = (*renderableIt)->GetMesh();

							if( mesh != 0 )
							{
								//bind the mesh's buffers
								try
								{
									//bind will throw if the mesh does not have a valid renderer or if
									//the buffers have not been initialized
									mesh->Bind( m_renderer );

								} catch( Kiwi::Exception& e )
								{
									//cant render this mesh, move on to the next one
									continue;
								}

								//render each subset of the mesh
								for( unsigned int i = 0; i < mesh->GetSubsetCount(); i++ )
								{
									//render this subset for all of the renderables
									for( ; renderableIt != meshItr->second.transparent_renderables.end(); renderableIt++ )
									{
										if( (*renderableIt)->GetParentEntity() && (*renderableIt)->GetParentEntity()->IsActive() == false )
										{
											//don't render any renderables that belong to an inactive entity
											continue;
										}

										//get the current subset
										Kiwi::Mesh::Subset* subset = mesh->GetSubset( i );
										//find the size of the subset
										unsigned int subsetSize = subset->endIndex - subset->startIndex;

										//set the renderable's current mesh subset so the shader knows what to render
										(*renderableIt)->SetCurrentMeshSubset( i );

										//set the renderable's shader parameters
										shader->SetObjectParameters( this, m_renderer->GetActiveRenderTarget(), *renderableIt );

										//now draw the renderable to the render target twice, once for the back faces
										//and then again for the front faces
										m_renderer->SetRasterState( L"Cull CW" );
										m_renderer->GetDeviceContext()->DrawIndexed( subsetSize, subset->startIndex, 0 );
										m_renderer->SetRasterState( L"Cull CCW" );
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
		}

	}

	void Scene::_Render2D( RenderableManager::RenderableShaderMap& shaderMap )
	{

		//render all of the 2D renderables
		auto shaderIt2D = shaderMap.shaderMap2D.begin();
		for( ; shaderIt2D != shaderMap.shaderMap2D.end(); shaderIt2D++ )
		{
			//get the shader from the shader container
			Kiwi::IShader* shader = m_shaderContainer.Find( shaderIt2D->first );

			if( shader != 0 )
			{
				//for each shader, if the shader exists, set it as active and get the mesh map
				shader->Bind( m_renderer );

				shader->SetFrameParameters( this );

				auto meshItr = shaderIt2D->second.meshMap.begin();
				for( ; meshItr != shaderIt2D->second.meshMap.end(); meshItr++ )
				{
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
							try
							{
								//bind will throw if the mesh does not have a valid renderer or if
								//the buffers have not been initialized
								mesh->Bind( m_renderer );
							} catch( Kiwi::Exception& e )
							{
								//cant render this mesh, move on to the next one
								continue;
							}

							//render each subset of the mesh
							for( unsigned int i = 0; i < mesh->GetSubsetCount(); i++ )
							{
								//render this subset for all of the renderables
								for( ; renderableIt != meshItr->second.opaque_renderables.end(); renderableIt++ )
								{
									if( (*renderableIt)->GetParentEntity() && (*renderableIt)->GetParentEntity()->IsActive() == false )
									{
										//don't render any renderables that belong to an inactive entity
										continue;
									}

									//get the current subset
									Kiwi::Mesh::Subset* subset = mesh->GetSubset( i );
									//find the size of the subset
									unsigned int subsetSize = subset->endIndex - subset->startIndex;

									//set the renderable's current mesh subset so the shader knows what to render
									(*renderableIt)->SetCurrentMeshSubset( i );

									//set the renderable's shader parameters
									shader->SetObjectParameters( this, m_renderer->GetActiveRenderTarget(), *renderableIt );

									//now draw the renderable to the render target
									m_renderer->GetDeviceContext()->DrawIndexed( subsetSize, subset->startIndex, 0 );
								}
							}
						} else
						{
							MessageBox( NULL, L"No 2D Mesh", L"A", MB_OK );
						}
					}
				}
			} else
			{
				MessageBox( NULL, L"No 2D Shader", L"A", MB_OK );
			}
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

	void Scene::SetPlayerEntity( Kiwi::Entity* playerEntity )
	{

		if( playerEntity == 0 ) return;

		if( m_playerEntity != 0 )
		{
			m_renderableManager.Remove( m_playerEntity->GetRenderable() );
			m_playerEntity = 0;
		}

		m_playerEntity = playerEntity;
		m_renderableManager.AddRenderable( m_playerEntity->GetRenderable() );

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