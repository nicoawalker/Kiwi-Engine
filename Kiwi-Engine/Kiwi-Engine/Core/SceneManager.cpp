#include "Utilities.h"
#include "Exception.h"
#include "SceneManager.h"
#include "Scene.h"
#include "EngineRoot.h"

//#include "../Graphics/GraphicsCore.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/RenderWindow.h"
#include "../Graphics/RenderTarget.h"

namespace Kiwi
{

	SceneManager::SceneManager( Kiwi::EngineRoot* engine )
	{

		m_engine = engine;
		if( m_engine == 0 ) 
		{ 
			throw Kiwi::Exception( L"SceneManager", L"Invalid engine pointer" ); 
		}

	}

	SceneManager::~SceneManager()
	{
		
		auto it = m_scenes.begin();
		for(; it != m_scenes.end(); )
		{
			SAFE_DELETE(it->second);
			it = m_scenes.erase(it);
		}

	}

	Kiwi::Scene* SceneManager::CreateScene(std::wstring name, std::wstring rendererName)
	{

		Kiwi::Scene* scene = 0;
		try
		{

			Kiwi::Renderer* renderer = m_engine->GetGraphicsManager()->FindRendererWithName( rendererName );

			scene = new Kiwi::Scene( m_engine, name, renderer );

			m_scenes[name] = scene;

			return scene;

		}catch(...)
		{
			throw;
		}

	}

};