#ifndef _KIWI_ENTITYMANAGER_H_
#define _KIWI_ENTITYMANAGER_H_

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Kiwi
{
	
	class Entity;

	class EntityManager
	{
	protected:

		/*stores the active entities (these are updated every frame)*/
		std::unordered_map<std::wstring, Kiwi::Entity*> m_entities;

	public:

		EntityManager();
		~EntityManager();

		void Update();
		void FixedUpdate();

		void AddEntity(Kiwi::Entity* entity);

		void DeleteEntity(Kiwi::Entity* entity);
		void DeleteEntityWithName(std::wstring name);
		void DeleteAllEntities();
		void DeleteAllInactiveEntities();

		Kiwi::Entity* FindEntityWithName(std::wstring name);
		std::vector<Kiwi::Entity*> FindEntitiesWithTag(std::wstring tag);

	};
};

#endif