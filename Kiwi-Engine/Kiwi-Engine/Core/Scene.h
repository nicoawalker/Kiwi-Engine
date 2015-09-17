#ifndef _KIWI_SCENE_H_
#define _KIWI_SCENE_H_

#include "IThreadSafe.h"
#include "Assert.h"
#include "AssetManager.h"
#include "Vector4.h"

#include <string>
#include <vector>
#include <mutex>

namespace Kiwi
{

	class Renderer;
	class Entity;
	class EntityManager;
	class SceneLoader;
	class IAsset;
	class Camera;
	class IRenderable;
	class RenderTarget;
	class RenderTargetManager;

	class Scene :
		public IThreadSafe
	{
	protected:

		std::wstring m_name;

		Kiwi::Renderer* m_renderer;

		Kiwi::RenderTargetManager* m_renderTargetManager;
		Kiwi::EntityManager* m_entityManager;
		Kiwi::AssetManager* m_assetManager;
		Kiwi::SceneLoader* m_sceneLoader;

		Kiwi::Vector4 m_diffuseLight;
		Kiwi::Vector4 m_diffuseDirection;
		Kiwi::Vector4 m_ambientLight;

	public:

		Scene(std::wstring name, Kiwi::Renderer* renderer);
		virtual ~Scene();

		virtual void Render() {}

		virtual void Load();

		virtual void AddEntity( Kiwi::Entity* entity );

		virtual void AddAsset( Kiwi::IAsset* asset );

		Kiwi::Renderer* GetRenderer()const { return m_renderer; }

		const Kiwi::Vector4& GetDiffuseLight()const { return m_diffuseLight; }
		const Kiwi::Vector4& GetDiffuseLightDirection()const { return m_diffuseDirection; }
		const Kiwi::Vector4& GetAmbientLight()const { return m_ambientLight; }

		virtual Kiwi::Entity* FindEntityWithName( std::wstring name );
		virtual std::vector<Kiwi::Entity*> FindEntitiesWithTag( std::wstring tag );

		template<class AssetType>
		AssetType* FindAssetWithName( std::wstring name );

		Kiwi::RenderTarget* FindRenderTargetWithName( std::wstring name );

	};

	template<class AssetType>
	AssetType* Scene::FindAssetWithName( std::wstring name )
	{

		assert( m_assetManager != 0 );

		return dynamic_cast<AssetType*>(m_assetManager->FindAssetWithName( name ));

	}

};

#endif