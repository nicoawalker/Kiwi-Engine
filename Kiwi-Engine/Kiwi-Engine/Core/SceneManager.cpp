#include "Utilities.h"
#include "Exception.h"
#include "SceneManager.h"
#include "Scene.h"
#include "EngineRoot.h"

#include "../Graphics/Renderer.h"
#include "..\Graphics\RenderWindow.h"
#include "../Graphics/RenderTarget.h"

namespace Kiwi
{

	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
		m_scenes.clear();
	}

	void SceneManager::Shutdown()
	{
		for( auto it = m_scenes.begin(); it != m_scenes.end(); it++ )
		{
			it->second->Shutdown();
		}
	}

	void SceneManager::Render()
	{
		for( auto& keyValuePair : m_scenes )
		{
			keyValuePair.second->Render();
		}
	}

	Kiwi::Scene* SceneManager::CreateScene( std::wstring sceneName, std::wstring rendererName )
	{
		if( this->FindScene( sceneName ) != 0 )
		{
			throw Kiwi::Exception( L"SceneManager::CreateScene", L"Failed to create scene '" + sceneName + L"', scene already exists", KIWI_EXCEPTION::DUPLICATIONATTEMPT );
		}

		Kiwi::Renderer* renderer = nullptr;

		if( rendererName.compare( L"" ) == 0 )
		{
			renderer = _kGraphics.GetActiveRenderer();
			if( renderer == nullptr )
			{
				throw Kiwi::Exception( L"SceneManager::CreateScene", L"Failed to create scene '" + sceneName + L"': no renderer was specified and no default renderer was set", KIWI_EXCEPTION::INVALIDPARAMETER );
			}

		} else
		{
			renderer = _kEngine.GetGraphicsManager().FindRenderer( rendererName );
			if( renderer == nullptr )
			{
				throw Kiwi::Exception( L"SceneManager::CreateScene", L"Failed to create scene '" + sceneName + L"': invalid renderer", KIWI_EXCEPTION::INVALIDPARAMETER );
			}
		}

		Kiwi::Scene* newScene = new Kiwi::Scene( sceneName, *renderer );
		if( newScene == NULL )
		{
			throw Kiwi::Exception( L"SceneManager::CreateScene", L"Allocation error creating new scene", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		m_scenes.insert( std::make_pair( sceneName, std::unique_ptr<Kiwi::Scene>(newScene) ) );

		return newScene;
	}

	Kiwi::Scene* SceneManager::FindScene( std::wstring name )
	{
		auto sceneItr = m_scenes.find( name );
		if( sceneItr != m_scenes.end() )
		{
			return sceneItr->second.get();
		}

		return 0;
	}

	std::vector<Kiwi::Scene*> SceneManager::GetSceneList()
	{
		std::vector<Kiwi::Scene*> sceneList;

		for( auto itr = m_scenes.begin(); itr != m_scenes.end(); itr++ )
		{
			sceneList.push_back( itr->second.get() );
		}

		return sceneList;
	}

	void SceneManager::DestroyScene( std::wstring name )
	{
		auto itr = m_scenes.find( name );
		if( itr != m_scenes.end() )
		{
			if( itr->second )
			{
				itr->second->Shutdown();
			}
		}
	}

};