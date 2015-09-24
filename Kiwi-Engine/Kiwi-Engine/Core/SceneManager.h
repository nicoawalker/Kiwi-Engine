#ifndef _KIWI_SCENEMANAGER_H_
#define _KIWI_SCENEMANAGER_H_

#include "IFrameEventListener.h"

#include <string>
#include <unordered_map>

namespace Kiwi
{

	class Scene;
	class Renderer;
	class RenderTarget;
	class EngineRoot;
	class GraphicsCore;

	class SceneManager:
		public Kiwi::IFrameEventListener
	{
	protected:

		Kiwi::EngineRoot* m_engine;

		std::unordered_map<std::wstring, Kiwi::RenderTarget*> m_renderTargets;

		std::unordered_map<std::wstring, Kiwi::Scene*> m_scenes;

	public:

		SceneManager( Kiwi::EngineRoot* engine );
		virtual ~SceneManager();

		Kiwi::Scene* CreateScene( std::wstring name, std::wstring rendererName );

		Kiwi::Scene* FindSceneWithName( std::wstring name );

		virtual void OnUpdate( const Kiwi::FrameEvent& evt );
		virtual void OnFixedUpdate( const Kiwi::FrameEvent& evt );

	};
};

#endif