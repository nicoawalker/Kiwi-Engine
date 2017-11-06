#include "GameObjectManager.h"

#include "..\Types.h"

#include <Windows.h>

namespace Kiwi
{

	GameObjectManager::GameObjectManager()
	{
		m_started = false;
		m_bufferNewObjects = false;
	}

	GameObjectManager::~GameObjectManager()
	{
		m_gameObjects.clear();
	}

	void GameObjectManager::_InitializeNewGameObjects()
	{
		/*enable buffering new objects that are added while we iterate over
		the unitializedObjects list*/
		m_bufferNewObjects = true;

		/*start all new gameobjects (this is done regardless if the gameobject is active or not, and 
		is only not performed if the gameobject is shutdown or already started)*/
		for( auto itr = m_unitializedObjects.begin(); itr != m_unitializedObjects.end(); )
		{
			if( (*itr) != nullptr && (*itr)->IsShutdown() == false && (*itr)->IsStarted() == false )
			{
				(*itr)->Start();
			}
			itr = m_unitializedObjects.erase( itr );
		}

		/*add any newly added game objects to the unitialized list*/
		for( auto itr = m_newObjectBuffer.begin(); itr != m_newObjectBuffer.end(); itr++ )
		{
			m_unitializedObjects.insert( *itr );
		}
		m_newObjectBuffer.clear();

		m_bufferNewObjects = false;
	}

	void GameObjectManager::Start()
	{
		m_started = true;
	}

	void GameObjectManager::Update()
	{
		if( m_started != true ) return;
		
		/*if new objects have been added since last frame, perform the 'first-run' tasks for each*/
		if( m_unitializedObjects.size() > 0 ) this->_InitializeNewGameObjects();

		for( auto itr = m_gameObjects.begin(); itr != m_gameObjects.end(); itr++ )
		{
			if( itr->second != 0 && itr->second->IsActive() && itr->second->IsShutdown() == false )
			{
				itr->second->Update();
			}
		}

	}

	void GameObjectManager::FixedUpdate()
	{
		if( m_started != true ) return;

		/*if new objects have been added since last frame, perform the 'first-run' tasks for each*/
		if( m_unitializedObjects.size() > 0 ) this->_InitializeNewGameObjects();

		for( auto itr = m_gameObjects.begin(); itr != m_gameObjects.end(); itr++ )
		{
			if( itr->second != 0 && itr->second->IsActive() && itr->second->IsShutdown() == false )
			{
				itr->second->FixedUpdate();
			}
		}

	}

	Kiwi::GameObject* GameObjectManager::ManageGameObject( std::unique_ptr<Kiwi::GameObject> gameObject )
	{
		if( gameObject == nullptr ) return nullptr;
		
		/*add the unique pointer to the game object list*/
		auto itr = m_gameObjects.insert( std::make_pair( gameObject->GetName(), std::move( gameObject ) ) );

		Kiwi::GameObject* obj = itr->second.get();

		if( m_started )
		{
			obj->Start();

		} else
		{
			if( m_bufferNewObjects )
			{
				m_newObjectBuffer.push_back( obj );

			} else
			{
				m_unitializedObjects.insert( obj );
			}
		}

		return obj;
	}

	void GameObjectManager::CleanUp()
	{
		for( auto itr = m_gameObjects.begin(); itr != m_gameObjects.end(); )
		{
			if( itr->second == 0 || (itr->second->IsShutdown() && itr->second->GetReferenceCount() == 0) )
			{
				itr = m_gameObjects.erase( itr );

				/*the game object may still be in the new object list, so check for it and remove it*/
				auto unitializedItr = m_unitializedObjects.find( itr->second.get() );
				if( unitializedItr != m_unitializedObjects.end() )
				{
					m_unitializedObjects.erase( unitializedItr );
				}

			} else
			{
				itr++;
			}
		}
	}

	void GameObjectManager::ShutdownAll()
	{
		auto itr = m_gameObjects.begin();
		for( ; itr != m_gameObjects.end(); itr++ )
		{
			if( itr->second != 0 && itr->second->IsShutdown() == false )
			{
				itr->second->Shutdown();
			}
		}
	}

	Kiwi::GameObject* GameObjectManager::Find( std::wstring name )
	{
		auto it = m_gameObjects.find( name );
		if( it != m_gameObjects.end() )
		{
			//found it
			return it->second.get();
		}

		return 0;
	}

	Kiwi::GameObjectList GameObjectManager::FindAll( std::wstring name )
	{
		Kiwi::GameObjectList matches;

		for( auto objItr = m_gameObjects.equal_range( name ).first; objItr != m_gameObjects.equal_range( name ).second; objItr++ )
		{
			matches.push_back( objItr->second.get() );
		}

		return matches;
	}

	Kiwi::GameObjectList GameObjectManager::FindAllWithTag( std::wstring tag )
	{
		std::vector<Kiwi::GameObject*> matches;

		for( auto& go : m_gameObjects )
		{
			if( go.second != 0 && go.second->HasTag( tag ) )
			{
				matches.push_back( go.second.get() );
			}
		}

		return matches;
	}

}