#ifndef _KIWI_EXCEPTION_H_
#define _KIWI_EXCEPTION_H_

#include <string>
#include <exception>

namespace Kiwi
{

	class Exception :
		public std::exception
	{
	protected:

		std::wstring m_source;
		std::wstring m_error;

	public:

		Exception(std::wstring source, std::wstring error);

		~Exception() {}

		std::wstring GetError()const { return m_error; }

		std::wstring GetSource()const { return m_source; }

	};
};

#endif