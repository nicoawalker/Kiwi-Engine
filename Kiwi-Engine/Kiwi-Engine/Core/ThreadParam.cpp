#include "ThreadParam.h"
#include "Utilities.h"
#include "EngineRoot.h"

namespace Kiwi
{

	ThreadParam::ThreadParam( std::wstring threadName )
	{

		m_threadName = threadName;
		m_shutdownFlag = false;
		m_UID = Kiwi::NewUID();

	}

	void ThreadParam::Lock()
	{

		m_threadMutex.lock();

	}

	void ThreadParam::Unlock()
	{

		m_threadMutex.unlock();

	}

	bool ThreadParam::TryLock()
	{

		return m_threadMutex.try_lock();

	}

	void ThreadParam::Synchronize()
	{

		/*block until the game engine's frame count is greater that the one stored in the thread param*/
		while( m_currentFrame >= _kEngine.GetGameTimer().GetFrameCount() )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		}

		/*update to the new frame*/
		m_currentFrame = _kEngine.GetGameTimer().GetFrameCount();

	}

}