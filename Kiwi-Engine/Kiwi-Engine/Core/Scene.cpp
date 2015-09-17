#include "Scene.h"
#include "Utilities.h"
#include "Exception.h"
#include "Assert.h"
#include "EntityManager.h"
#include "AssetManager.h"
#include "SceneLoader.h"

#include "../Graphics/Renderer.h"
#include "../Graphics/IShader.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/IRenderable.h"
#include "../Graphics/RenderTargetManager.h"

#include "../Graphics/D3D11Interface.h"
#include "../Graphics/DepthStencil.h"

namespace Kiwi
{

	Scene::Scene(std::wstring name, Kiwi::Renderer* renderer)
	{

		if(renderer == 0)
		{
			throw Kiwi::Exception(L"Scene", L"Invalid renderer");
		}

		try
		{
			m_name = name;

			m_renderer = renderer;

			m_diffuseLight = Kiwi::Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
			m_diffuseDirection = Kiwi::Vector4( 1.0f, -1.0f, 0.0f, 0.0f );
			m_ambientLight = Kiwi::Vector4( 0.4f, 0.4f, 0.4f, 1.0f );

			m_renderTargetManager = new Kiwi::RenderTargetManager();
			m_entityManager = new Kiwi::EntityManager();
			m_assetManager = new Kiwi::AssetManager();
			m_sceneLoader = new Kiwi::SceneLoader( this );

			//add the back buffer render target to the render target manager
			Kiwi::RenderTarget* backBuffer = m_renderer->GetBackBuffer();
			m_renderTargetManager->AddRenderTarget( backBuffer );

		} catch( const Kiwi::Exception& e )
		{
			throw e;
		}

	}

	Scene::~Scene()
	{

		m_renderer = 0;

		SAFE_DELETE( m_entityManager );
		SAFE_DELETE( m_assetManager );
		SAFE_DELETE( m_sceneLoader );

	}

	void Scene::Load()
	{

		m_sceneLoader->QueueLoading<Kiwi::Mesh>( L"StaticMesh", L"TestMesh", L"H:\\Programming\\Projects\\Kiwi-Engine-Demo\\Kiwi Engine Demo\\Kiwi Engine Demo\\Data\\Models\\Old Farm House\\Farmhouse OBJ.obj" );

		m_sceneLoader->Start();
		while( !m_sceneLoader->Finished() )
		{

		}

		if( m_sceneLoader->GetExceptions().size() != 0 )
		{
			throw m_sceneLoader->GetExceptions()[0];
		}

	}

	void Scene::AddEntity( Kiwi::Entity* entity )
	{

		assert( m_entityManager != 0 );

		try
		{
			m_entityManager->AddEntity( entity );

		} catch( ... )
		{
			throw;
		}

	}

	void Scene::AddAsset( Kiwi::IAsset* asset )
	{

		assert( m_assetManager != 0 );

		try
		{
			m_assetManager->AddAsset( asset );

		} catch( ... )
		{
			throw;
		}

	}

	Kiwi::Entity* Scene::FindEntityWithName( std::wstring name )
	{

		assert( m_entityManager != 0 );

		return m_entityManager->FindEntityWithName( name );

	}

	std::vector<Kiwi::Entity*> Scene::FindEntitiesWithTag( std::wstring tag )
	{

		assert( m_entityManager != 0 );

		return m_entityManager->FindEntitiesWithTag( tag );

	}

	Kiwi::RenderTarget* Scene::FindRenderTargetWithName( std::wstring name )
	{

		return ( m_renderTargetManager ) ? m_renderTargetManager->FindRenderTargetWithName( name ) : 0;

	}

};