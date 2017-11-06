#ifndef _KIWI_UIDPOOL_H_
#define _KIWI_UIDPOOL_H_

#include "..\Core\Utilities.h"

#include <vector>

namespace Kiwi
{
	template<typename IDType>
	class UIDPool
	{
	protected:

		/*stores available ids*/
		std::vector<IDType> m_freePool;

		/*stores unavailable/used ids*/
		std::vector<IDType> m_usedPool;

	public:

		void Initialize( std::vector<IDType>& pool )
		{
			m_freePool = pool;
		}

		void Clear()
		{
			Kiwi::FreeMemory( m_freePool );
			Kiwi::FreeMemory( m_usedPool );
		}

		void Reset()
		{
			m_freePool.insert( m_freePool.end(), m_usedPool.begin(), m_usedPool.end() );
			Kiwi::FreeMemory( m_usedPool );
		}

		bool Get( IDType& _id )
		{
			if( m_freePool.size() > 0 )
			{
				_id = m_freePool.back();
				m_freePool.pop_back();
				m_usedPool.push_back( _id );
				return true;

			} else
			{
				return false;
			}
		}

		void Release( IDType& id )
		{
			auto itr = std::find( m_usedPool.begin(), m_usedPool.end(), id );
			if( itr != m_usedPool.end() )
			{
				std::swap( itr, m_usedPool.end() - 1 );
				m_usedPool.pop_back();
				m_freePool.push_back( id );
			}
		}

	};
}

#endif