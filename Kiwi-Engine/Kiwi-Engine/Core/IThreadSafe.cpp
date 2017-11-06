#include "IThreadSafe.h"

namespace Kiwi
{

	IThreadSafe::~IThreadSafe()
	{
		SAFE_DELETE( m_mutex );
	}

	void IThreadSafe::MutexLock()
	{
		if( m_mutex ) m_mutex->lock();
	}

	void IThreadSafe::MutexUnlock()
	{
		if( m_mutex ) m_mutex->unlock();
	}

}