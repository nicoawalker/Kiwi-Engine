#include "Entity.h"
#include "Transform.h"
#include "Utilities.h"
#include "Scene.h"
#include "EngineRoot.h"

#include "../Graphics/Mesh.h"

#include "../Physics/Rigidbody.h"

namespace Kiwi
{

	Entity::Entity( std::wstring name, Kiwi::Scene& scene ):
		Kiwi::GameObject( name, L"Entity", scene ),
		m_transform( scene, *this )
	{
		m_isActive = true;
		m_parent = 0;
		m_entityType = ENTITY_3D;

		m_componentMask = (long)Kiwi::ComponentType::Transform;
	}

	Entity::~Entity()
	{
		if( !m_isShutdown )
		{
			//shutdown and free any active children
			for( auto childItr = m_childEntities.begin(); childItr != m_childEntities.end(); childItr++ )
			{
				if( childItr->second != 0 )
				{
					//free the entity so it may be deleted
					childItr->second->Free();
					childItr->second->Shutdown();
				}
			}

			//destroy components
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second != 0 )
				{
					compItr->second->Shutdown();
				}
			}
		}

		Kiwi::FreeMemory( m_components );
		Kiwi::FreeMemory( m_childEntities );
	}

	Kiwi::Component* Entity::_AttachComponent( Kiwi::ComponentType componentType )
	{
		if( componentType == Kiwi::ComponentType::Transform )return &m_transform;

		if( this->HasComponent( componentType ) )
		{
			/*detach existing component of the same type*/
			this->_DetachComponent( componentType );
		}

		/*create the new component*/
		std::unique_ptr<Kiwi::Component> component = this->_CreateComponent( componentType );
		if( component == nullptr ) return nullptr;

		auto insResult = m_components.insert( std::make_pair( static_cast<long>(componentType), std::move( component ) ) );
		if( insResult.second == false )
		{
			component->Shutdown();
			return nullptr;
		}

		//add component to component mask
		m_componentMask |= static_cast<long>(componentType);

		Kiwi::Component* attachedComponent = insResult.first->second.get();

		attachedComponent->_SetEntity( this );
		attachedComponent->_OnAttached();
		if( m_started )
		{
			/*if the entity has already been started, all new components should be started once attached,
			otherwise they will all be started once the entity is started*/
			attachedComponent->Start();
		}

		if( componentType == Kiwi::ComponentType::Rigidbody )
		{
			Kiwi::Rigidbody* rigidbody = dynamic_cast<Kiwi::Rigidbody*>(attachedComponent);
			if( rigidbody != nullptr )
			{
				m_scene->GetPhysicsSystem()->AddRigidbody( rigidbody );
			}
		}

		this->_OnComponentAttached( attachedComponent );

		return attachedComponent;
	}

	std::unique_ptr<Kiwi::Component> Entity::_CreateComponent( Kiwi::ComponentType componentType )
	{
		switch( componentType )
		{
			case Kiwi::ComponentType::Rigidbody:
				{
					return std::make_unique<Kiwi::Rigidbody>( *m_scene );
					break;
				}
			case Kiwi::ComponentType::Mesh:
				{
					return std::make_unique<Kiwi::Mesh>( *m_scene, m_objectName + L"/Mesh" );
					break;
				}
			default:break;
		}

		return nullptr;
	}

	void Entity::_DetachComponent( Kiwi::ComponentType componentType )
	{
		auto compItr = m_components.find( static_cast<long>(componentType) );
		if( compItr != m_components.end() )
		{
			Kiwi::Component* component = compItr->second.get();

			/*check if the new component is a mesh, transform, or rigidbody to replace the current one*/
			if( componentType == Kiwi::ComponentType::Rigidbody )
			{
				Kiwi::Rigidbody* rigidbody = dynamic_cast<Kiwi::Rigidbody*>(component);
				if( rigidbody == nullptr ) return;

				m_scene->GetPhysicsSystem()->RemoveRigidbody( rigidbody );
			}

			this->_OnComponentDetached( component );
			if( component->IsShutdown() == false )
			{
				component->_OnDetached();
				component->_SetEntity( nullptr );
				component->Shutdown();
			}
			m_components.erase( compItr );

			//remove component from component mask
			m_componentMask &= ~(static_cast<long>(componentType));
		}
	}

	Kiwi::Component* Entity::_FindComponent( Kiwi::ComponentType componentType )
	{
		auto compItr = m_components.find( static_cast<long>(componentType) );
		if( compItr != m_components.end() )
		{
			return compItr->second.get();
		}

		if( componentType == Kiwi::ComponentType::Transform ) return &m_transform;

		return nullptr;
	}

	void Entity::_OnActivate()
	{
		for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
		{
			itr->second->SetActive( true );
		}

		m_transform.SetActive( true );
		for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
		{
			compItr->second->SetActive( true );
		}
	}

	void Entity::_OnDeactivate()
	{
		for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
		{
			itr->second->SetActive( false );
		}

		m_transform.SetActive( false );
		for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
		{
			compItr->second->SetActive( false );
		}
	}

	void Entity::_SetParent( Kiwi::Entity* parent )
	{
		if( m_parent == parent ) return;

		if( m_parent != nullptr )
		{
			m_parent->DetachChild( this );
		}

		m_parent = parent;
	}

	void Entity::AttachChild( Kiwi::Entity* entity )
	{
		if( entity == nullptr ) return;

		m_childEntities.insert( std::make_pair( entity->GetName(), entity ) );
		entity->_SetParent( this );
		entity->Reserve(); //reserve the entity so it will not be deleted until it is released
	}

	Kiwi::Entity* Entity::DetachChild( Kiwi::Entity* entity )
	{
		if( entity == nullptr ) return nullptr;

		for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
		{
			if( itr->second == entity )
			{
				Kiwi::Entity* ent = itr->second;

				if( ent != nullptr )
				{
					ent->m_parent = nullptr;
					ent->Free();
				}
				m_childEntities.erase( itr );

				return  ent;
			}
		}

		return nullptr;
	}

	Kiwi::Entity* Entity::DetachChildWithName( std::wstring name )
	{
		auto it = m_childEntities.find( name );
		if( it != m_childEntities.end() )
		{
			Kiwi::Entity* ent = it->second;
			if( ent != nullptr )
			{
				ent->m_parent = nullptr;
				ent->Free();
			}
			m_childEntities.erase( it );

			return ent;

		} else
		{//wasnt found, try the same for all children

			for( auto itr = m_childEntities.begin(); itr != m_childEntities.end(); itr++ )
			{
				if( itr->second != nullptr )
				{
					Kiwi::Entity* ent = itr->second->DetachChildWithName( name );
					if( ent != nullptr )
					{
						return ent;
					}
				}
			}
		}

		return 0;
	}

	void Entity::DetachComponent( Kiwi::ComponentType componentType )
	{
		/*transforms cannot be detached from entities*/
		if( componentType == Kiwi::ComponentType::Transform ) return;

		this->_DetachComponent( componentType );
	}

	Kiwi::Entity* Entity::FindChildWithName( std::wstring name )
	{
		auto it = m_childEntities.find( name );
		if( it != m_childEntities.end() )
		{
			return it->second;

		} else
		{
			return 0;
		}
	}

	std::vector<Kiwi::Entity*> Entity::FindChildrenWithTag( std::wstring tag )
	{
		std::vector<Kiwi::Entity*> matches;

		for( const auto& it : m_childEntities )
		{
			if( it.second->HasTag( tag ) )
			{
				matches.push_back( it.second );
			}
		}

		return matches;
	}

	void Entity::FirstUpdate()
	{
		GameObject::FirstUpdate();

		/*update all components as well*/
		m_transform.FirstUpdate();
		for( auto itr = m_components.begin(); itr != m_components.end(); itr++ )
		{
			itr->second->FirstUpdate();
		}
	}

	void Entity::FixedUpdate()
	{
		if( !m_isShutdown && m_isActive )
		{
			this->_OnFixedUpdate();

			m_transform.FixedUpdate();
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second->IsShutdown() == false && compItr->second->IsActive() == true )
				{
					compItr->second->FixedUpdate();
				}
			}
		}
	}

	bool Entity::HasChild()
	{
		return (m_childEntities.size() > 0) ? true : false;
	}

	bool Entity::HasComponent( Kiwi::ComponentType componentType )
	{
		return m_componentMask & static_cast<long>(componentType);
	}

	void Entity::Shutdown()
	{
		if( !m_isShutdown )
		{
			//shutdown child entities
			for( auto childItr = m_childEntities.begin(); childItr != m_childEntities.end(); childItr++ )
			{
				if( childItr->second != 0 )
				{
					childItr->second->Shutdown();
					childItr->second->Free(); //free the entity so it may be deleted
				}
			}

			//shutdown components
			m_transform.Shutdown();
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second != 0 )
				{
					compItr->second->Shutdown();
				}
			}

			this->_OnShutdown();

			m_isShutdown = true;
			m_isActive = false;
			m_componentMask = 0;
		}
	}

	void Entity::Start()
	{
		if( m_started ) return;

		GameObject::Start();

		/*start all components as well, child entities will be started elsewhere*/
		m_transform.Start();
		for( auto itr = m_components.begin(); itr != m_components.end(); itr++ )
		{
			itr->second->Start();
		}
	}

	void Entity::Update()
	{
		if( !m_isShutdown && m_isActive )
		{
			this->_OnUpdate();

			m_transform.Update();
			//update all components
			for( auto compItr = m_components.begin(); compItr != m_components.end(); compItr++ )
			{
				if( compItr->second->IsShutdown() == false && compItr->second->IsActive() == true )
				{
					compItr->second->Update();
				}
			}
		}
	}

};