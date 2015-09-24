#ifndef _KIWI_ENTITY_H_
#define _KIWI_ENTITY_H_

#include "Transform.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace Kiwi
{
	class IRenderable;
	//class Rigidbody;
	class Scene;

	class Entity
	{
	public:

		enum EntityType { ENTITY_3D, ENTITY_2D, ENTITY_LIGHT, ENTITY_SOUND };

	protected:

		EntityType m_entityType;

		Kiwi::Scene* m_scene;

		Kiwi::Entity* m_parent;

		Kiwi::Transform m_transform;

		Kiwi::IRenderable* m_renderable;

		/*unique identifier for the entity*/
		std::wstring m_name;

		/*optional, non-unique identifiers*/
		std::unordered_set<std::wstring> m_tags;

		/*map of child entities, sorted by name*/
		std::unordered_map<std::wstring, Kiwi::Entity*> m_childEntities;

		/*only active entities are updated and rendered*/
		bool m_isActive;

	public:

		Entity(std::wstring name, Kiwi::Scene* scene);
		virtual ~Entity() = 0; //pure virtual destructor to keep entity abstract

		virtual void Shutdown() {}

		virtual void Update() {}

		virtual void FixedUpdate() {}

		virtual void AddTag(std::wstring tag) { m_tags.insert(tag); }

		/*removes the matching string from the entity*/
		virtual void RemoveTag(std::wstring tag) { m_tags.erase(tag); }
		/*removes all tags from the entity*/
		virtual void RemoveAllTags();

		virtual Kiwi::Entity* FindChildWithName(std::wstring name);
		virtual std::vector<Kiwi::Entity*> FindChildrenWithTag(std::wstring tag);

		virtual void AttachChild(Kiwi::Entity* entity);

		/*detaches the entity (does not destroy it)*/
		virtual void DetachChild(Kiwi::Entity* entity);
		/*detaches the entity with the matching name (does not destroy it)*/
		virtual void DetachChildWithName(std::wstring name);

		/*checks if the entity has a certain tag*/ 
		virtual bool HasTag(std::wstring tag);
		/*checks if the entity has any children*/
		virtual bool HasChild();

		/*attaches this entity to the target parent entity*/
		virtual void SetParent(Kiwi::Entity* parent);

		virtual void SetActive( bool active ) { m_isActive = active; }

		virtual Kiwi::IRenderable* GetRenderable()const { return m_renderable; }
		virtual Kiwi::Transform* GetTransform() { return &m_transform; }
		virtual Entity::EntityType GetType()const { return m_entityType; }
		virtual Kiwi::Scene* GetScene()const { return m_scene; }
		virtual std::wstring GetName()const { return m_name; }
		virtual bool IsActive()const { return m_isActive; }

	};
};

#endif