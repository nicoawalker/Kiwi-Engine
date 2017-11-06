#include "AssetLoadQueue.h"

namespace Kiwi
{

	/*add information required to load a new asset to the queue*/
	void AssetLoadQueue::Enqueue( const Kiwi::StringPair& assetInformation )
	{
		std::lock_guard<std::mutex> lock( m_queueMutex );

		m_queuedAssets.push_back( assetInformation );
		m_condv.notify_one();
	}

	bool AssetLoadQueue::PopNext( Kiwi::StringPair& resultDest, unsigned int timeout )
	{
		std::unique_lock<std::mutex> lock( m_queueMutex );

		/*if there are currently no assets to be loaded, wait for 'timeout' milliseconds
		for a new asset to be added and for the condition variable to be notified*/
		if( m_queuedAssets.size() == 0 )
		{
			m_condv.wait_for( lock, std::chrono::milliseconds( timeout ) );
		}

		/*if there are assets, return and remove the first one from the queue*/
		if( m_queuedAssets.size() > 0 )
		{
			resultDest = m_queuedAssets.front();
			m_queuedAssets.pop_front();

			return true;
		}

		//no assets, return false
		return false;
	}

	/*empties the queue, discarding all queued assets*/
	void AssetLoadQueue::Clear()
	{
		std::lock_guard<std::mutex> lock( m_queueMutex );

		m_queuedAssets.clear();
	}

}