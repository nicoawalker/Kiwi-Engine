#include "Entity.h"
#include "Transform.h"
#include "Utilities.h"

#include "../Graphics/IRenderable.h"

namespace Kiwi
{

	Entity::Entity(std::wstring name, Kiwi::Scene* scene)
	{

		m_name = name;
		m_isActive = true;
		m_scene = scene;
		m_parent = 0;
		m_renderable = 0;
		m_transform = Kiwi::Transform();
		m_entityType = ENTITY_3D;

	}

	Entity::~Entity()
	{

		Kiwi::FreeMemory(m_childEntities);
		Kiwi::FreeMemory(m_tags);
		
		SAFE_DELETE( m_renderable );

	}

	Kiwi::Entity* Entity::FindChildWithName(std::wstring name)
	{

		auto it = m_childEntities.find(name);
		if(it != m_childEntities.end())
		{
			return it->second;
		}else
		{
			return 0;
		}

	}

	std::vector<Kiwi::Entity*> Entity::FindChildrenWithTag(std::wstring tag)
	{

		std::vector<Kiwi::Entity*> matches;

		for( const auto& it : m_childEntities)
		{
			if(it.second->HasTag(tag))
			{
				matches.push_back(it.second);
			}
		}

		return matches;

	}

	void Entity::RemoveAllTags()
	{

		//empty the tag list and free the memory
		Kiwi::FreeMemory(m_tags);

	}

	void Entity::AttachChild(Kiwi::Entity* entity)
	{

		if(entity == 0) return;

		m_childEntities[entity->GetName()] = entity;

	}

	void Entity::DetachChild(Kiwi::Entity* entity)
	{

		if(entity == 0) return;

		auto it = m_childEntities.find(entity->GetName());
		if(it != m_childEntities.end())
		{
			it->second->SetParent(0);
			m_childEntities.erase(it);
		}

	}

	void Entity::DetachChildWithName(std::wstring name)
	{

		auto it = m_childEntities.find(name);
		if(it != m_childEntities.end())
		{
			it->second->SetParent(0);
			m_childEntities.erase(it);
		}

	}

	void Entity::SetParent(Kiwi::Entity* parent)
	{

		if(m_parent != 0)
		{
			m_parent->DetachChild(this);
			m_parent = 0;
		}

		m_parent = parent;

		if(m_parent) m_parent->AttachChild(this);

	}

	/*checks if the entity has a certain tag*/ 
	bool Entity::HasTag(std::wstring tag)
	{

		auto it = m_tags.find(tag);
		if(it != m_tags.end())
		{
			return true;
		}else
		{
			return false;
		}

	}

	bool Entity::HasChild()
	{

		if(m_childEntities.size() > 0)
		{
			return true;
		}else
		{
			return false;
		}

	}

};