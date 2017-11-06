#ifndef _KIWI_SCENEMANAGER_H_
#define _KIWI_SCENEMANAGER_H_

#include "Scene.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Kiwi
{

	class Renderer;
	class RenderTarget;
	class EngineRoot;
	class GraphicsCore;

	class SceneManager
	{
	protected:

		//std::unordered_map<std::wstring, Kiwi::RenderTarget*> m_renderTargets;

		std::unordered_map<std::wstring, std::unique_ptr<Kiwi::Scene>> m_scenes;

	public:

		SceneManager();
		virtual ~SceneManager();

		void Shutdown();

		void Render();

		/*creates a new scene. will either return a pointer to the created scene or throw a Kiwi::Exception
		|-sceneName: unique name for the newly created scene
		|-rendererName: name of the renderer the scene will use. if left blank the default renderer will be 
		  used, provided one has been set*/
		Kiwi::Scene* CreateScene( std::wstring sceneName, std::wstring rendererName = L"" );

		Kiwi::Scene* FindScene( std::wstring name );

		/*returns a vector containing all scenes in the scene manager*/
		std::vector<Kiwi::Scene*> GetSceneList();

		void DestroyScene( std::wstring name );

	};
};

#endif