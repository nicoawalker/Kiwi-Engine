#include "IAssetEventQueueReader.h"

namespace Kiwi
{

	void IAssetEventQueueReader::_QueueEvent( const AssetEvent& evt )
	{

		if( this->_OnAssetEventImm( evt ) == false )
		{
			m_assetEventQueue.push_back( evt );
		}

	}

	bool IAssetEventQueueReader::_PullNextAssetEvent()
	{

		if( m_assetEventQueue.size() == 0 ) return false;

		Kiwi::AssetEvent evt = m_assetEventQueue.front();
		m_assetEventQueue.pop_front();

		this->_OnAssetEvent( evt );

		return true;

	}

}