#ifndef _KIWI_ENTITY_H_
#define _KIWI_ENTITY_H_

#include "GameObject.h"
#include "Transform.h"
#include "Component.h"

#include "../Types.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace Kiwi
{

	class Scene;
	class EntityManager;
	class Mesh;
	class Entity;

	typedef std::unordered_multimap<std::wstring, Kiwi::Entity*> EntityMap;
	typedef std::vector<Kiwi::Entity*> EntityList;

	class Entity:
		public Kiwi::GameObject
	{
	friend class Scene;
	friend class EntityManager;
	public:

		enum EntityType { ENTITY_3D, ENTITY_2D, ENTITY_LIGHT, ENTITY_SOUND };

	protected:

		/*bitwise mask that stores which components the entity has for fast lookup*/
		long m_componentMask;

		EntityType m_entityType;

		Kiwi::Entity* m_parent;

		Kiwi::Transform m_transform;

		/*map of child entities, sorted by name*/
		std::unordered_multimap<std::wstring, Kiwi::Entity*> m_childEntities;

		std::unordered_map<long, std::unique_ptr<Component>> m_components;

	protected:

		virtual Kiwi::Component* _AttachComponent( Kiwi::ComponentType componentType );

		virtual std::unique_ptr<Kiwi::Component> _CreateComponent( Kiwi::ComponentType componentType );

		void _DetachComponent( Kiwi::ComponentType componentType );

		Kiwi::Component* _FindComponent( Kiwi::ComponentType componentType );

		virtual void _OnActivate();

		virtual void _OnDeactivate();

		virtual void _OnComponentAttached( Kiwi::Component* component ) {}

		virtual void _OnComponentDetached( Kiwi::Component* component ) {}

		/*attaches this entity to the target parent entity*/
		virtual void _SetParent( Kiwi::Entity* parent );

	public:

		Entity(std::wstring name, Kiwi::Scene& scene);
		virtual ~Entity();

		virtual void AttachChild( Kiwi::Entity* entity );

		/*detaches the entity (does not destroy it)*/
		virtual Kiwi::Entity* DetachChild( Kiwi::Entity* entity );

		/*detaches the entity with the matching name (does not destroy it)*/
		virtual Kiwi::Entity* DetachChildWithName( std::wstring name );

		/*if a component of the matching type is attached, it is detached and destroyed*/
		void DetachComponent( Kiwi::ComponentType componentName );

		virtual Kiwi::Entity* FindChildWithName( std::wstring name );

		virtual std::vector<Kiwi::Entity*> FindChildrenWithTag( std::wstring tag );

		virtual void FirstUpdate();

		virtual void FixedUpdate();

		virtual const EntityMap& GetChildren() { return m_childEntities; }

		Kiwi::Entity* GetParent()const { return m_parent; }

		virtual Kiwi::Scene* GetScene()const { return m_scene; }

		virtual Entity::EntityType GetType()const { return m_entityType; }
		
		/*checks if the entity has any children*/
		virtual bool HasChild();

		/*returns whether or not a component of a certain type is attached to the entity*/
		bool HasComponent( Kiwi::ComponentType componentType );

		virtual void SetEntityType( EntityType entityType ) { m_entityType = entityType; }

		virtual void Shutdown();

		virtual void Start();

		virtual void Update();

		/*--template member functions--*/

		template<class ComponentType>
		ComponentType* AttachComponent( Kiwi::ComponentType componentType )
		{
			return dynamic_cast<ComponentType*>(this->_AttachComponent( componentType ));
		}

		template<class ChildType>
		ChildType* FindChild( const std::wstring& childName )
		{
			auto it = m_childEntities.find( name );
			if( it != m_childEntities.end() )
			{
				return dynamic_cast<ChildType*>(it->second);
			}

			return 0;
		}

		template<class ComponentType>
		ComponentType* FindComponent( Kiwi::ComponentType componentType )
		{
			return dynamic_cast<ComponentType*>(this->_FindComponent( componentType ));
		}

		template<>
		Kiwi::Transform* FindComponent<Kiwi::Transform>( Kiwi::ComponentType componentType )
		{
			return &m_transform;
		}

	};
};

#endif