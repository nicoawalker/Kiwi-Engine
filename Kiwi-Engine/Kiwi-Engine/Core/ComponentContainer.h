#ifndef _KIWI_COMPONENTCONTAINER_H_
#define _KIWI_COMPONENTCONTAINER_H_

#include "../Core/Utilities.h"

#include <string>
#include <unordered_map>

namespace Kiwi
{

	template<typename SortType, class ComponentType>
	class ComponentContainer
	{
	protected:

		std::unordered_map<SortType, ComponentType*> m_components;

	public:

		ComponentContainer() {}
		virtual ~ComponentContainer()
		{
			auto it = m_components.begin();
			for( ; it != m_components.end();)
			{
				SAFE_DELETE( it->second );
				it = m_components.erase( it );
			}
			Kiwi::FreeMemory( m_components );
		}

		virtual typename std::unordered_map<SortType, ComponentType*>::iterator Front()
		{
			return m_components.begin();
		}

		virtual typename std::unordered_map<SortType, ComponentType*>::iterator Back()
		{
			return m_components.end();
		}

		virtual void Add( SortType key, ComponentType* value )
		{
			if( this->Find( key ) != 0 )
			{//names must be unique so delete any existing component with the same name
				this->Destroy( key );
			}
			m_components[key] = value;
		}

		virtual ComponentType* Find( SortType key )
		{
			auto it = m_components.find( key );
			if( it != m_components.end() )
			{
				return it->second;
			}

			return 0;
		}

		virtual void DestroyAll()
		{
			auto it = m_components.begin();
			for( ; it != m_components.end(); )
			{
				SAFE_DELETE( it->second );
			}

			Kiwi::FreeMemory( m_components );
		}

		virtual void Destroy( ComponentType* component )
		{
			auto it = m_components.begin();
			for( ; it != m_components.end(); it++ )
			{
				if( it->second == component )
				{
					SAFE_DELETE( it->second );
					it = m_components.erase( it );
					return;
				}
			}
		}

		virtual void Destroy( SortType key )
		{
			auto it = m_components.find( key );
			if( it != m_components.end() )
			{
				SAFE_DELETE( it->second );
				m_components.erase( it );
			}
		}

		/*removes the component with matching name, but does not delete its memory*/
		virtual ComponentType* Remove( SortType key )
		{
			auto it = m_components.find( key );
			if( it != m_components.end() )
			{
				ComponentType* comp = it->second;
				m_components.erase( it );
				return comp;
			}

			return 0;
		}

		/*removes the component, but does not delete its memory*/
		virtual void Remove( ComponentType* component )
		{
			auto it = m_components.begin();
			for( ; it != m_components.end(); it++ )
			{
				if( it->second == component )
				{
					it = m_components.erase( it );
					return;
				}
			}
		}

		unsigned int GetCount()
		{
			return m_components.size();
		}

	};
}

#endif