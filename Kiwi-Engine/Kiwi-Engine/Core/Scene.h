#ifndef _KIWI_SCENE_H_
#define _KIWI_SCENE_H_

#include "EntityManager.h"
#include "IThreadSafe.h"
#include "Assert.h"
#include "Vector4.h"
#include "GameObjectManager.h"

#include "../Events/EventHandling.h"
#include "..\Events\ISceneEventEmitter.h"

#include <string>
#include <vector>
#include <mutex>

namespace Kiwi
{

	class Renderer;
	class Entity;
	class IAsset;
	class Camera;
	class RenderTarget;
	class SceneManager;
	class ITerrain;
	class PhysicsSystem;
	class RenderWindow;
	class UICanvas;

	class Scene :
		public Kiwi::IThreadSafe,
		public Kiwi::ISceneEventEmitter,
		public Kiwi::IFrameEventReceiver
	{
	friend class SceneManager;
	protected:

		std::wstring m_name;

		/*renderer the scene will use for all render operations*/
		Kiwi::Renderer* m_renderer;

		/*render window that the scene is displayed on*/
		Kiwi::RenderWindow* m_renderWindow;

		/*a pointer to the engine's physics system*/
		Kiwi::PhysicsSystem* m_physicsSystem;

		/*manager that updates game objects as well as controls their lifetimes*/
		Kiwi::GameObjectManager m_gameObjectManager;

		/*manages entities and the entity renderqueue for the scene*/
		Kiwi::EntityManager m_entityManager;

		Kiwi::Vector4 m_diffuseLight;
		Kiwi::Vector4 m_diffuseDirection;
		Kiwi::Vector4 m_ambientLight;

		std::recursive_mutex m_sceneMutex;

		/*how often, in seconds, the scene should perform garbage collection*/
		double m_gcInterval;

		bool m_shutdown;
		bool m_isActive;
		bool m_started;

	protected:

		void _OnUpdate();
		void _OnFixedUpdate();

		Kiwi::EventResponse _OnFrameEvent( std::shared_ptr<Kiwi::FrameEvent> evt );

		void _GarbageCollect();

	public:

		Scene( std::wstring name, Kiwi::Renderer& renderer);
		virtual ~Scene();

		void Shutdown();

		void Start();

		void Render();

		/*transfers ownership of the game object to the scene's gameobject manager*/
		Kiwi::GameObject* AddGameObject( std::unique_ptr<Kiwi::GameObject> gameObject );

		Kiwi::Entity* CreateEntity( std::wstring name );

		Kiwi::UICanvas* CreateCanvas( std::wstring name, std::wstring camera, const Kiwi::Vector2d& dimensions );

		Kiwi::Camera* CreateCamera( std::wstring name );
		Kiwi::Camera* CreateCamera( std::wstring name, float FOV, float aspectRatio, float nearClip, float farClip );

		bool Raycast( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits );

		void SetActive( bool isActive );

		/*sets how often, in milliseconds, the scene should perform garbage collection on shutdown game objects*/
		void SetGCInterval( double interval ) { m_gcInterval = interval; }

		std::wstring GetName()const { return m_name; }

		Kiwi::Renderer* GetRenderer()const { return m_renderer; }
		Kiwi::RenderWindow* GetRenderWindow()const { return m_renderWindow; }

		const Kiwi::Vector4& GetDiffuseLight()const { return m_diffuseLight; }
		const Kiwi::Vector4& GetDiffuseLightDirection()const { return m_diffuseDirection; }
		const Kiwi::Vector4& GetAmbientLight()const { return m_ambientLight; }

		Kiwi::PhysicsSystem* GetPhysicsSystem()const { return m_physicsSystem; }

		bool IsShutdown()const { return m_shutdown; }
		bool IsActive()const { return m_isActive; }

		/*returns the first found entity with the given name, or 0 if no match is found*/
		Kiwi::Entity* FindEntity( std::wstring name );

		/*returns a vector containing all entities that have the given name*/
		EntityList FindAllEntitiesWithName( std::wstring name );

		/*returns a vector containing all entities with the given tag*/
		EntityList FindEntitiesWithTag( std::wstring tag );

		/*returns the camera with matching name, if it exists*/
		Kiwi::Camera* FindCamera( std::wstring name );

		/*returns the first found game object with the given name, or 0 if no match is found*/
		Kiwi::GameObject* FindObject( std::wstring name );

		/*returns a vector containing all game objects that have the given name*/
		GameObjectList FindAllObjectsWithName( std::wstring name );

		/*returns a vector containing all game objects with the given tag*/
		GameObjectList FindObjectsWithTag( std::wstring tag );

		template<typename EntityType>
		EntityType* FindEntity( std::wstring name )
		{
			return dynamic_cast<EntityType*>(m_entityManager.Find( name ));
		}

	};

};

#endif