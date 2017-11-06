#include "GPUBufferManager.h"

#include "../Core/EngineRoot.h"

namespace Kiwi
{

	GPUBufferManager::~GPUBufferManager()
	{
		Kiwi::FreeMemory( m_buffers );
	}

	Kiwi::IGPUBuffer* GPUBufferManager::_FindBuffer( long uid )
	{
		auto itr = m_buffers.find( uid );
		if( itr != m_buffers.end() )
		{
			return itr->second.get();
		}

		return nullptr;
	}

	Kiwi::IGPUBuffer* GPUBufferManager::FindBuffer( long uid )
	{
		std::lock_guard<std::mutex> lock( m_gpuBufferManager_M );

		return this->_FindBuffer( uid );
	}

	void GPUBufferManager::Update()
	{
		static double timer = 0.0;
		timer += _kGameTimer.GetDeltaTime();

		/*do garbage collection every 5 seconds*/
		if( timer >= 5000.0 )
		{
			timer -= 5000.0;

			for( auto itr = m_buffers.begin(); itr != m_buffers.end(); )
			{
				if( itr->second->GetReferenceCount() == 0 )
				{
					itr = m_buffers.erase( itr );

				} else
				{
					itr++;
				}
			}
		}
	}

	void GPUBufferManager::Shutdown()
	{
		Kiwi::FreeMemory( m_buffers );
	}

}