#include "EntityManager.h"
#include "Entity.h"
#include "Exception.h"
#include "Utilities.h"
#include "GameObjectManager.h"

#include "../Graphics/RenderQueue.h"
#include "..\Graphics\Mesh.h"

#include <algorithm>

namespace Kiwi
{

	EntityManager::EntityManager( Kiwi::Scene& scene )
	{
		m_scene = &scene;
	}

	EntityManager::~EntityManager()
	{
		/*need to release all entities*/
		for( auto itr = m_entities.begin(); itr != m_entities.end(); itr++ )
		{
			if( itr->second != nullptr )
			{
				itr->second->Free(); //decrement the reference count
			}
		}
	}

	/*Kiwi::RenderQueue* EntityManager::GenerateRenderQueue()
	{

		m_renderQueue.Generate( m_entities );
		return &m_renderQueue;

	}*/

	std::vector<Kiwi::Mesh*> EntityManager::GenerateMeshRenderList()
	{
		std::vector<Kiwi::Mesh*> meshList;

		for( auto& keyValuePair : m_entities )
		{
			Kiwi::Entity* entity = keyValuePair.second;

			if( entity != nullptr && entity->IsActive() && entity->IsShutdown() == false )
			{
				Kiwi::Mesh* mesh = entity->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
				if( mesh != nullptr && mesh->IsActive() && mesh->IsShutdown() == false )
				{
					meshList.push_back( mesh );
				}
			}
		}

		return meshList;
	}

	/*void EntityManager::ClearRenderQueue()
	{

		m_renderQueue.Clear();

	}*/

	void EntityManager::Clear()
	{
		/*need to release all entities before removing them all*/
		for( auto itr = m_entities.begin(); itr != m_entities.end(); itr++ )
		{
			if( itr->second != 0 )
			{
				itr->second->Free(); //decrement the reference count
			}
		}

		m_entities.clear();
	}

	void EntityManager::ManageEntity( Kiwi::Entity* entity )
	{
		if( entity == 0 ) return;

		entity->Reserve(); //increment reference count so entity will not be destroyed until freed

		m_entities.insert( std::make_pair( entity->GetName(), entity ) );
	}

	void EntityManager::Raytrace( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits )
	{
		auto itr = m_entities.begin();
		for( ; itr != m_entities.end(); itr++ )
		{
			if( itr->second != 0 && itr->second->IsShutdown() == false && itr->second->IsActive() && itr->second->HasTag(L"terrain") == false )
			{
				Kiwi::Mesh* mesh = itr->second->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
				if( mesh != 0 && mesh->IsShutdown() == false && mesh->IsActive() == true )
				{
					Kiwi::Transform* transform = itr->second->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
					if( transform != 0 && transform->GetSquareDistance(origin) <= maxDepthFromOrigin * maxDepthFromOrigin )
					{
						std::vector<Kiwi::Mesh::Face> tIntersect;
						mesh->IntersectRay( origin, direction, maxDepthFromOrigin, tIntersect, true );
						if( tIntersect.size() > 0 )
						{
							hits.push_back( itr->second );
						}
					}
				}
			}
		}

		//sort the entities based on distance from the origin
		std::sort( hits.begin(), hits.end(), [origin]( Kiwi::Entity* e1, Kiwi::Entity* e2 )
		{
			Kiwi::Transform* m1T = e1->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
			Kiwi::Transform* m2T = e2->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );

			if( m1T && m2T )
			{
				return m1T->GetSquareDistance( origin ) < m2T->GetSquareDistance( origin );

			} else
			{
				return true;
			}
		} );
	}

	void EntityManager::ShutdownAll()
	{
		for( auto it = m_entities.begin(); it != m_entities.end(); it++ )
		{
			if( it->second != 0 )
			{
				it->second->Shutdown();
			}
		}
	}

	void EntityManager::CleanUp()
	{
		for( auto sdItr = m_entities.begin(); sdItr != m_entities.end();)
		{
			if( sdItr->second == 0 || sdItr->second->IsShutdown() )
			{
				if( sdItr->second != 0 ) sdItr->second->Free(); //decrement the reference count

				sdItr = m_entities.erase( sdItr );

			} else
			{
				sdItr++;
			}
		}
	}

	Kiwi::Entity* EntityManager::Find(std::wstring name)
	{
		auto it = m_entities.find(name);
		if(it != m_entities.end())
		{
			//found it
			return it->second;
		}

		return 0;
	}

	Kiwi::EntityList EntityManager::FindAll( std::wstring name )
	{
		Kiwi::EntityList matches;

		for( auto objItr = m_entities.equal_range( name ).first; objItr != m_entities.equal_range( name ).second; objItr++ )
		{
			matches.push_back( objItr->second );
		}

		return matches;
	}

	Kiwi::EntityList EntityManager::FindAllWithTag(std::wstring tag)
	{
		Kiwi::EntityList matches;

		for(auto& entity : m_entities)
		{
			if( entity.second != 0 && entity.second->HasTag(tag))
			{
				matches.push_back( entity.second );
			}
		}

		return matches;
	}

};