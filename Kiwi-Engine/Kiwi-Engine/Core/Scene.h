#ifndef _KIWI_SCENE_H_
#define _KIWI_SCENE_H_

#include "EntityManager.h"
#include "IThreadSafe.h"
#include "Assert.h"
#include "AssetManager.h"
#include "Vector4.h"
#include "IFrameEventListener.h"
#include "ComponentContainer.h"

#include "../Graphics/RenderableManager.h"
#include "../Graphics/RenderTargetManager.h"
#include "../Graphics/IShader.h"

#include <string>
#include <vector>
#include <mutex>

namespace Kiwi
{

	class Renderer;
	class Entity;
	class SceneLoader;
	class IAsset;
	class Camera;
	class IRenderable;
	class RenderTarget;

	class Scene :
		public Kiwi::IThreadSafe,
		public Kiwi::IFrameEventListener
	{
	protected:

		Kiwi::EngineRoot* m_engine;

		std::wstring m_name;

		Kiwi::Renderer* m_renderer;

		Kiwi::RenderTargetManager m_renderTargetManager;
		Kiwi::RenderableManager m_renderableManager;
		Kiwi::EntityManager m_entityManager;
		Kiwi::AssetManager m_assetManager;

		Kiwi::SceneLoader* m_sceneLoader;

		//contains all of the scene's shaders, sorted by name
		Kiwi::ComponentContainer<std::wstring, Kiwi::IShader> m_shaderContainer;

		Kiwi::Vector4 m_diffuseLight;
		Kiwi::Vector4 m_diffuseDirection;
		Kiwi::Vector4 m_ambientLight;

	public:

		Scene( Kiwi::EngineRoot* engine, std::wstring name, Kiwi::Renderer* renderer);
		virtual ~Scene();

		virtual void Load();

		virtual void Render();

		virtual void AddEntity( Kiwi::Entity* entity );
		virtual void AddAsset( Kiwi::IAsset* asset );
		virtual void AddShader( Kiwi::IShader* shader );

		Kiwi::Renderer* GetRenderer()const { return m_renderer; }

		const Kiwi::Vector4& GetDiffuseLight()const { return m_diffuseLight; }
		const Kiwi::Vector4& GetDiffuseLightDirection()const { return m_diffuseDirection; }
		const Kiwi::Vector4& GetAmbientLight()const { return m_ambientLight; }

		virtual Kiwi::Entity* FindEntityWithName( std::wstring name );
		virtual std::vector<Kiwi::Entity*> FindEntitiesWithTag( std::wstring tag );

		template<class AssetType>
		AssetType* FindAssetWithName( std::wstring name );

		Kiwi::RenderTarget* FindRenderTargetWithName( std::wstring name );

		virtual void OnUpdate( const Kiwi::FrameEvent& evt ) { this->Render(); }
		virtual void OnFixedUpdate( const Kiwi::FrameEvent& evt ) {}

	};

	template<class AssetType>
	AssetType* Scene::FindAssetWithName( std::wstring name )
	{

		return dynamic_cast<AssetType*>(m_assetManager.FindAssetWithName( name ));

	}

};

#endif