#include "EntityManager.h"
#include "Entity.h"
#include "Utilities.h"

namespace Kiwi
{

	EntityManager::EntityManager()
	{

	}

	EntityManager::~EntityManager()
	{

		auto it = m_entities.begin();
		for(; it != m_entities.end();)
		{
			SAFE_DELETE(it->second);
			it = m_entities.erase(it);
		}

	}

	void EntityManager::Update()
	{

		for( auto entity : m_entities )
		{
			if( entity.second->IsActive() )
			{
				entity.second->Update();
			}
		}

	}

	void EntityManager::FixedUpdate()
	{

		for(auto entity : m_entities)
		{
			if(entity.second->IsActive())
			{
				entity.second->FixedUpdate();
			}
		}

	}

	void EntityManager::AddEntity(Kiwi::Entity* entity)
	{

		if(entity == 0) return;

		m_entities[entity->GetName()] = entity;

		/*switch( entity->GetType() )
		{
			case Entity::ENTITY_3D:
				{



					break;
				}
			case Entity::ENTITY_2D:
				{

					break;
				}
			case Entity::ENTITY_LIGHT:
				{

					break;
				}
			case Entity::ENTITY_SOUND:
				{
					break;
				}
			default: break;
		}*/

	}

	void EntityManager::Destroy(Kiwi::Entity* entity)
	{

		if(entity == 0) return;

		this->DestroyWithName(entity->GetName());

	}

	void EntityManager::DestroyWithName(std::wstring name)
	{

		auto it = m_entities.find(name);
		if(it != m_entities.end())
		{
			//found it, delete it
			SAFE_DELETE(it->second);
			m_entities.erase(it);
		}

	}

	void EntityManager::DestroyAll()
	{

		auto it = m_entities.begin();
		for(; it != m_entities.end();)
		{
			SAFE_DELETE(it->second);
			it = m_entities.erase(it);
		}

	}

	void EntityManager::DestroyInactive()
	{

		auto inactiveIt = m_entities.begin();
		for(; inactiveIt != m_entities.end();)
		{
			if( !inactiveIt->second->IsActive() )
			{
				SAFE_DELETE( inactiveIt->second );
				inactiveIt = m_entities.erase( inactiveIt );
			}
		}

	}

	Kiwi::Entity* EntityManager::FindWithName(std::wstring name)
	{

		auto it = m_entities.find(name);
		if(it != m_entities.end())
		{
			//found it
			return it->second;
		}

		return 0;

	}

	std::vector<Kiwi::Entity*> EntityManager::FindAllWithTag(std::wstring tag)
	{

		std::vector<Kiwi::Entity*> matches;

		for(auto entity : m_entities)
		{
			if(entity.second->HasTag(tag))
			{
				matches.push_back(entity.second);
			}
		}

		return matches;

	}

};