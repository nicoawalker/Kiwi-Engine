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

		int m_exceptionID;

	public:

		Exception( std::wstring source, std::wstring error, int exceptionID = 0 );

		~Exception() {}

		std::wstring GetError()const { return m_error; }

		std::wstring GetSource()const { return m_source; }

		std::wstring ToString();

		int GetID()const { return m_exceptionID; }

	};

};

#define KIWI_ASSERT(cond, srcStr, errMsg, excpType) { if( !(cond) ) { throw Kiwi::Exception(srcStr, errMsg, excpType); } }

enum KIWI_EXCEPTION
{
	NOTINITIALIZED, //attempted to use/start the engine before it was properly initialized
	INVALIDINDEX, //tried to access an element in an array with an invalid index
	INVALIDTHREADPARAM, //passed a parameter to a thread that was different than what the thread expected
	FILEOPENFAILURE, //tried to open a file but failed
	INVALIDRENDERGROUP, //tried to render a mesh that belongs to a non-existent render group
	NOPARENT, //tried to do something with a component that had no parent
	NULLOPERATION, //tried to do some operation with a null pointer
	OBJECTCREATIONFAILED, //creating a new object using 'new' produced a null pointer
	OBJECTALLOCFAILED, //'new' failed to allocate memory for a new object
	DUPLICATIONATTEMPT, //attempted to create some object that already exists
	BADCAST, //a type cast failed
	NOTFOUND, //requested object was not found
	INVALIDSTRING, //string parameter provided was invalid
	TIMEOUT, //action being waited for timed out
	UNKNOWNERROR, //error not known
	INVALIDTHREAD, //requested thread was not found
	INVALIDPARAMETER, //a parameter was missing or incorrect
	ACCESSDENIED, //access was denied. for example if a thread lock could not be obtained
};

#endif