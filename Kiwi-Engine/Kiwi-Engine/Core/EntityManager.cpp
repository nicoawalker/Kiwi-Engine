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

		auto it = m_entities.begin();
		for(; it != m_entities.end();)
		{
			if(it->second->IsActive())
			{
				it->second->Update();
				it++;
			}
		}

	}

	void EntityManager::FixedUpdate()
	{

		for(auto entity : m_entities)
		{
			if(entity.second->IsActive())
			{
				entity.second->Update();
			}
		}

	}

	void EntityManager::AddEntity(Kiwi::Entity* entity)
	{

		if(entity == 0) return;

		m_entities[entity->GetName()] = entity;

	}

	void EntityManager::DeleteEntity(Kiwi::Entity* entity)
	{

		if(entity == 0) return;

		this->DeleteEntityWithName(entity->GetName());

	}

	void EntityManager::DeleteEntityWithName(std::wstring name)
	{

		auto it = m_entities.find(name);
		if(it != m_entities.end())
		{
			//found it, delete it
			SAFE_DELETE(it->second);
			m_entities.erase(it);
		}

	}

	void EntityManager::DeleteAllEntities()
	{

		auto it = m_entities.begin();
		for(; it != m_entities.end();)
		{
			SAFE_DELETE(it->second);
			it = m_entities.erase(it);
		}

	}

	void EntityManager::DeleteAllInactiveEntities()
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

	Kiwi::Entity* EntityManager::FindEntityWithName(std::wstring name)
	{

		auto it = m_entities.find(name);
		if(it != m_entities.end())
		{
			//found it
			return it->second;
		}

		return 0;

	}

	std::vector<Kiwi::Entity*> EntityManager::FindEntitiesWithTag(std::wstring tag)
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