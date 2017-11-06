#include "Exception.h"
#include "Utilities.h"

#include "../Types.h"

namespace Kiwi
{

	Exception::Exception(std::wstring source, std::wstring error, int exceptionID )
	{
		m_source = source;
		m_error = error;
		m_exceptionID = exceptionID;

		KIWIDEBUGSTRING( Kiwi::ToWString(L"Exception generated:\n > " + m_source + L" " + m_error + L"\n").c_str() );
	}

	std::wstring Exception::ToString()
	{
		return Kiwi::ToWString( L"[" + m_source + L"] " + m_error + L"\n" );
	}

};