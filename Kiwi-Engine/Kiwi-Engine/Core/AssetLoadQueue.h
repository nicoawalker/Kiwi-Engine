#ifndef _KIWI_ASSETLOADQUEUE_H_
#define _KIWI_ASSETLOADQUEUE_H_

#include "..\Types.h"

#include <mutex>
#include <deque>

namespace Kiwi
{
	/*thread-safe queue of assets that are to be loaded, shared between the asset manager and all running asset load threads.
	threads pop items from the queue, attempt to load them, then return the result to the asset manager*/
	class AssetLoadQueue
	{
	protected:

		std::deque<Kiwi::StringPair> m_queuedAssets;

		std::mutex m_queueMutex;
		std::condition_variable m_condv;

	public:

		AssetLoadQueue() {}

		AssetLoadQueue( const std::vector<Kiwi::StringPair>& assets ) :
			m_queuedAssets( std::deque<Kiwi::StringPair>( assets.begin(), assets.end() ) )
		{
		}

		AssetLoadQueue( const std::deque<Kiwi::StringPair>& assetQueue ) :
			m_queuedAssets( assetQueue )
		{
		}

		/*add information required to load a new asset to the queue*/
		void Enqueue( const Kiwi::StringPair& assetInformation );

		/*attemps to pop the next queued asset definition from the queue, and returns true if one is found. 
		if there are no currently queued assets, the function will block for up to 'timeout' milliseconds for 
		a new asset to become available to the calling thread. if no asset it added during that time the 
		function will return false*/
		bool PopNext( Kiwi::StringPair& resultDest, unsigned int timeout );

		/*empties the queue, discarding all queued assets*/
		void Clear();

		/*returns the number of items in the queue*/
		unsigned int GetAssetCount()const { return (unsigned int)m_queuedAssets.size(); }

	};
}

#endif