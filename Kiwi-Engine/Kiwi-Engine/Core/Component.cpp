#include "Component.h"
#include "Entity.h"
#include "Utilities.h"
#include "Scene.h"

namespace Kiwi
{

	Component::~Component()
	{

		if( !m_isShutdown )
		{
			MessageBox( NULL, m_objectName.c_str(), L"Component destroyed without being shutdown", MB_OK );
		}

		Kiwi::FreeMemory( m_childComponents );

	}

	Kiwi::Component* Component::_AttachComponent( std::unique_ptr<Kiwi::Component> component )
	{

		if( component )
		{
			Kiwi::Component* existingComponent = this->_FindComponent( component->GetName() );
			if( existingComponent != 0 )
			{//component already exists
				return existingComponent;

			} else
			{
				Kiwi::Component* attachedComponent = component.get();

				m_childComponents.insert( std::make_pair( component->GetName(), std::move( component ) ) );
				attachedComponent->_SetEntity( m_entity );
				if( m_started )
				{
					/*if the entity has already been started, all new components should be started once attached,
					otherwise they will all be started once the entity is started*/
					attachedComponent->Start();
				}

				return attachedComponent;
			}
		}

		return 0;
	}

	void Component::_DetachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			auto compItr = m_childComponents.find( component->GetName() );
			if( compItr != m_childComponents.end() )
			{
				compItr->second->_OnDetached();

				/*shutdown and erase the component. it will be deleted by the scene*/
				compItr->second->Shutdown();
				m_childComponents.erase( compItr );
			}
		}

	}

	void Component::_SetEntity( Kiwi::Entity* entity )
	{

		m_entity = entity;

		for( auto compItr = m_childComponents.begin(); compItr != m_childComponents.end(); compItr++ )
		{
			if( compItr->second != 0 )
			{
				compItr->second->_SetEntity( m_entity );
			}
		}

	}

	void Component::FixedUpdate()
	{
		if( !m_isShutdown && m_isActive )
		{
			for( auto compItr = m_childComponents.begin(); compItr != m_childComponents.end(); compItr++ )
			{
				compItr->second->FixedUpdate();
			}

			this->_OnFixedUpdate();
		}
	}

	void Component::Update()
	{
		if( !m_isShutdown && m_isActive )
		{
			//update all child components
			for( auto compItr = m_childComponents.begin(); compItr != m_childComponents.end(); compItr++ )
			{
				compItr->second->Update();
			}

			this->_OnUpdate();
		}
	}

	void Component::Shutdown()
	{

		if( !m_isShutdown )
		{
			m_isShutdown = true;
			m_isActive = false;

			this->_OnShutdown();

			//shutdown the child components
			for( auto itr = m_childComponents.begin(); itr != m_childComponents.end(); itr++ )
			{
				itr->second->Shutdown();
			}
		}

	}

	Kiwi::Component* Component::AttachComponent( std::wstring componentName, std::wstring componentType )
	{

		return nullptr;// this->_AttachComponent( m_scene->CreateComponent( componentName, componentType ) );

	}

	Kiwi::Component* Component::AttachComponent( std::wstring componentName, std::wstring componentType, Kiwi::StringPair stringParams )
	{

		return nullptr;// this->_AttachComponent( m_scene->CreateComponent( componentName, componentType, stringParams ) );

	}

	Kiwi::Component* Component::AttachComponent( std::unique_ptr<Kiwi::Component> component )
	{

		return this->_AttachComponent( std::move( component ) );

	}

	/*Kiwi::Component* Component::AttachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			this->_AttachComponent( component );
		}

		return component;

	}*/

	void Component::DetachComponent( Kiwi::Component* component )
	{

		if( component )
		{
			this->_DetachComponent( component );
		}

	}

	void Component::DetachComponent( std::wstring componentName )
	{

		auto compItr = m_childComponents.find( componentName );
		if( compItr != m_childComponents.end() )
		{
			compItr->second->_OnDetached();

			/*shutdown and erase the component*/
			compItr->second->Shutdown();
			m_childComponents.erase( compItr );
		}

	}

	Kiwi::Component* Component::_FindComponent( const std::wstring& key )
	{
		auto compItr = m_childComponents.find( key );
		if( compItr != m_childComponents.end() )
		{
			return compItr->second.get();
		}

		return 0;
	}

}