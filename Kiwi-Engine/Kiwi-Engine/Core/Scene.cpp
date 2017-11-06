#include "Scene.h"
#include "Utilities.h"
#include "Exception.h"
#include "Assert.h"
#include "EntityManager.h"
#include "AssetManager.h"
#include "EngineRoot.h"
#include "ITerrain.h"

#include "..\Utilities\Timer.h"

#include "../Graphics/Renderer.h"
#include "..\Graphics\IShader.h"
#include "../Graphics/Mesh.h"
#include "..\Graphics\InstancedMesh.h"
#include "../Graphics/Camera.h"
#include "..\Graphics\D3D11Interface.h"
#include "../Graphics/DepthStencil.h"
#include "..\Graphics\RenderQueue.h"

#include "../Graphics/UI/UICanvas.h"

namespace Kiwi
{

	Scene::Scene( std::wstring name, Kiwi::Renderer& renderer ) :
		m_entityManager( *this )
	{

		m_shutdown = false;
		m_isActive = false;
		m_started = false;
		m_name = name;
		m_renderer = &renderer;
		m_renderWindow = m_renderer->GetRenderWindow();
		m_gcInterval = 10; //garbage collect every 10 seconds

		m_diffuseLight = Kiwi::Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
		m_diffuseDirection = Kiwi::Vector4( 1.0f, -1.0f, 0.0f, 0.0f );
		m_ambientLight = Kiwi::Vector4( 0.4f, 0.4f, 0.4f, 1.0f );

	}

	Scene::~Scene()
	{
	}

	Kiwi::EventResponse Scene::_OnFrameEvent( std::shared_ptr<Kiwi::FrameEvent> evt )
	{
		if( !m_isActive ) Kiwi::EventResponse::NONE;

		switch( evt->eventType )
		{
			case FrameEventType::FIXED_UPDATE:
				{
					this->_OnFixedUpdate();
					break;
				}
			case FrameEventType::UPDATE:
				{
					this->_OnUpdate();
					break;
				}
			default: break;
		}

		return Kiwi::EventResponse::NONE;
	}

	void Scene::_GarbageCollect()
	{
		m_entityManager.CleanUp();

		/*destroys all game objects that have been shut down and whos reference counts are zero*/
		m_gameObjectManager.CleanUp();
	}

	void Scene::_OnUpdate()
	{
		this->_EmitSceneEventQueue();

		m_gameObjectManager.Update();

		/*timer used to indicate when the scene should perform garbage collection on shutdown game objects*/
		static double gcTimer = 0.0;
		gcTimer += _kEngine.GetGameTimer().GetDeltaTime();
		if( gcTimer >= m_gcInterval )
		{
			gcTimer -= m_gcInterval;
			this->_GarbageCollect();
		}
	}

	void Scene::_OnFixedUpdate()
	{
		m_gameObjectManager.FixedUpdate();
	}

	void Scene::Start()
	{
		if( m_started == false )
		{
			/*starts all game objects currently in the scene*/
			m_gameObjectManager.Start();

			/*activate the scene and register it with the engine root to receive frame events*/
			this->SetActive( true );

			this->EmitSceneEvent( std::make_shared<Kiwi::SceneEvent>( *this, L"SceneStarted", Kiwi::SceneEventType::SCENE_STARTED ) );

			m_started = true;
		}
	}

	void Scene::Render()
	{
		Kiwi::Timer timer;
		m_renderer->SetActiveScene( *this );

		//m_renderer->SubmitMeshes( m_entityManager.GenerateMeshRenderList() );

		timer.Start();

		m_renderer->Render();

		double gameTime1 = timer.Milliseconds();
		timer.Start();

		m_renderer->Present();

		double gameTime2 = timer.Milliseconds();

		KIWIDEBUGSTRING( Kiwi::ToWString( L"Time1: " + Kiwi::ToWString( gameTime1 ) +
										  L" Time 2: " + Kiwi::ToWString( gameTime2 ) + L"\n" ).c_str() );
	}

	void Scene::Shutdown()
	{
		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		_kEngine.DisconnectFrameEventReceiver( *this );

		this->EmitSceneEvent( std::make_shared<Kiwi::SceneEvent>( *this, L"SceneShutdown", Kiwi::SceneEventType::SCENE_SHUTDOWN ) );

		m_entityManager.ShutdownAll();
		m_gameObjectManager.ShutdownAll();

		m_shutdown = true;
		m_started = false;
	}

	Kiwi::GameObject* Scene::AddGameObject( std::unique_ptr<Kiwi::GameObject> object )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		/*give ownership of the entity to the game object manager*/
		Kiwi::GameObject* go = m_gameObjectManager.ManageGameObject( std::move( object ) );

		/*if the game object is an entity, it has to also be added to the entity manager*/
		Kiwi::Entity* e = dynamic_cast<Kiwi::Entity*>(go);
		if( e != 0 )
		{
			/*add a pointer to the entity to the entity manager*/
			m_entityManager.ManageEntity( e );
		}

		return go;
	}

	Kiwi::Entity* Scene::CreateEntity( std::wstring name )
	{
		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		return dynamic_cast<Kiwi::Entity*>(this->AddGameObject( std::make_unique<Kiwi::Entity>( name, *this ) ));
	}

	Kiwi::UICanvas* Scene::CreateCanvas( std::wstring name, std::wstring camera, const Kiwi::Vector2d& dimensions )
	{
		Kiwi::UICanvas* existing = this->FindEntity<Kiwi::UICanvas>( name );
		if( existing != 0 ) return existing;

		Kiwi::Camera* cam = this->FindCamera( camera );

		std::unique_ptr<Kiwi::UICanvas> canvas = std::make_unique<Kiwi::UICanvas>( *this, name, dimensions, cam );
		Kiwi::UICanvas* cPtr = canvas.get();

		this->AddGameObject( std::move( canvas ) );

		return cPtr;
	}

	Kiwi::Camera* Scene::CreateCamera( std::wstring name )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		return dynamic_cast<Kiwi::Camera*>(this->AddGameObject( std::make_unique<Kiwi::Camera>( name, *this ) ));

	}

	Kiwi::Camera* Scene::CreateCamera( std::wstring name, float FOV, float aspectRatio, float nearClip, float farClip )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		return dynamic_cast<Kiwi::Camera*>(this->AddGameObject( std::make_unique<Kiwi::Camera>( name, *this, FOV, aspectRatio, nearClip, farClip ) ));

	}

	bool Scene::Raycast( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits )
	{

		m_entityManager.Raytrace( origin, direction, maxDepthFromOrigin, hits );

		return (hits.size() > 0) ? true : false;

	}

	void Scene::SetActive( bool isActive )
	{

		if( isActive != m_isActive )
		{
			if( isActive )
			{
				_kEngine.ConnectFrameEventReceiver( *this, L"", &Scene::_OnFrameEvent );
				m_isActive = true;

			} else
			{
				_kEngine.DisconnectFrameEventReceiver( *this );
				m_isActive = false;
			}
		}

	}

	Kiwi::Entity* Scene::FindEntity( std::wstring name )
	{

		return m_entityManager.Find( name );

	}

	Kiwi::EntityList Scene::FindAllEntitiesWithName( std::wstring name )
	{

		return m_entityManager.FindAll( name );

	}

	Kiwi::Camera* Scene::FindCamera( std::wstring name )
	{

		return dynamic_cast<Kiwi::Camera*>(this->FindEntity( name ));

	}

	Kiwi::EntityList Scene::FindEntitiesWithTag( std::wstring tag )
	{

		return m_entityManager.FindAllWithTag( tag );

	}

	Kiwi::GameObject* Scene::FindObject( std::wstring name )
	{

		return m_gameObjectManager.Find( name );

	}

	Kiwi::GameObjectList Scene::FindAllObjectsWithName( std::wstring name )
	{

		return m_gameObjectManager.FindAll( name );

	}

	Kiwi::GameObjectList Scene::FindObjectsWithTag( std::wstring tag )
	{

		return m_gameObjectManager.FindAllWithTag( tag );

	}

};