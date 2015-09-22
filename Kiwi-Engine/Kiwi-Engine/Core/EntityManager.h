#ifndef _KIWI_ENTITYMANAGER_H_
#define _KIWI_ENTITYMANAGER_H_

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Kiwi
{

	class Renderer;

	class EntityManager
	{
	private:

		struct LightMap
		{
			//stores the name of a render target, and a vector of light entities bound to the render target
			std::unordered_map<std::wstring, std::vector<Kiwi::Entity*>> map;
		};

	protected:

		/*stores the active entities (these are updated every frame)*/
		std::unordered_map<std::wstring, Kiwi::Entity*> m_entities;

		//stores all of the lights active in the scene
		LightMap m_lights;

	public:

		EntityManager();
		~EntityManager();

		void Update();
		void FixedUpdate();

		void AddEntity(Kiwi::Entity* entity);

		void Destroy(Kiwi::Entity* entity);
		void DestroyWithName(std::wstring name);
		void DestroyAll();
		void DestroyInactive();

		Kiwi::Entity* FindWithName(std::wstring name);
		std::vector<Kiwi::Entity*> FindAllWithTag(std::wstring tag);

	};
};

#endif