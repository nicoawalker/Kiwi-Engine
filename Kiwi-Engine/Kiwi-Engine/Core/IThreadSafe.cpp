#include "IThreadSafe.h"

namespace Kiwi
{

	IThreadSafe::~IThreadSafe()
	{
		SAFE_DELETE( m_mutex );
	}

}