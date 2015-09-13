#ifndef _KIWI_THREADSAFE_H_
#define _KIWI_THREADSAFE_H_

#include "Utilities.h"

#include <mutex>

namespace Kiwi
{

	class IThreadSafe
	{
	public:

		std::mutex* m_mutex;

	public:

		IThreadSafe()
		{
			m_mutex = new std::mutex();
		}

		virtual ~IThreadSafe() = 0;

	};

}

#endif