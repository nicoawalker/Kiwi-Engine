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

		m_engine->AddListener( this );

	}

	SceneManager::~SceneManager()
	{
		
		if( m_engine ) m_engine->RemoveListener( this );

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

	Kiwi::Scene* SceneManager::FindSceneWithName( std::wstring name )
	{

		auto sceneItr = m_scenes.find( name );
		if( sceneItr != m_scenes.end() )
		{
			return sceneItr->second;
		}

		return 0;

	}

	void SceneManager::OnUpdate( const Kiwi::FrameEvent& evt )
	{

		auto sceneItr = m_scenes.begin();
		for( ; sceneItr != m_scenes.end(); sceneItr++ )
		{
			sceneItr->second->Update();
		}

	}

	void SceneManager::OnFixedUpdate( const Kiwi::FrameEvent& evt )
	{

		auto sceneItr = m_scenes.begin();
		for( ; sceneItr != m_scenes.end(); sceneItr++ )
		{
			sceneItr->second->FixedUpdate();
		}

	}

};