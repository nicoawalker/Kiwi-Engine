#include "Exception.h"

namespace Kiwi
{

	Exception::Exception(std::wstring source, std::wstring error)
	{

		m_source = source;
		m_error = error;

	}

};