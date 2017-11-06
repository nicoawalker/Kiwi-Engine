#ifndef _KIWI_ENTITYMANAGER_H_
#define _KIWI_ENTITYMANAGER_H_

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Kiwi
{

	class Renderer;
	class Scene;

	class EntityManager
	{
	private:

		struct LightMap
		{
			//stores the name of a render target, and a vector of light entities bound to the render target
			std::unordered_multimap<std::wstring, std::vector<Kiwi::Entity*>> map;
		};

	protected:

		Kiwi::Scene* m_scene;

		/*stores the entities, with the entity name as the key*/
		Kiwi::EntityMap m_entities;

		//stores all of the lights active in the scene
		LightMap m_lights;

		//Kiwi::RenderQueue m_renderQueue;

	public:

		EntityManager( Kiwi::Scene& scene );
		~EntityManager();

		/*generates a new render queue based on the entities currently managed by the entity manager*/
		//Kiwi::RenderQueue* GenerateRenderQueue();

		/*clears the render queue and all it's render groups so that it contains no entities*/
		//void ClearRenderQueue();

		/*returns a list of meshes to be rendered this frame*/
		std::vector<Kiwi::Mesh*> GenerateMeshRenderList();

		/*removes all entities from the entity manager*/
		void Clear();
		
		/*adds an existing entity to the entity manager*/
		void ManageEntity(Kiwi::Entity* entity);

		void Raytrace( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits );

		/*calls the shutdown method on all entities in the manager*/
		void ShutdownAll();

		/*removes all shut down entities from the entity manager*/
		void CleanUp();

		/*returns the first found entity with the given name, or 0 if no match is found*/
		Kiwi::Entity* Find(std::wstring name);

		/*returns a vector containing all entities that have the given name*/
		Kiwi::EntityList FindAll( std::wstring name );

		/*returns a vector containing all entities with the given tag*/
		Kiwi::EntityList FindAllWithTag(std::wstring tag);

		//Kiwi::RenderQueue* GetRenderQueue() { return &m_renderQueue; }

	};
};

#endif