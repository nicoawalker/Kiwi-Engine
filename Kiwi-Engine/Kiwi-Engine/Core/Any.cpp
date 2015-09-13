#include "Any.h"

namespace Kiwi
{

	Any::Any()
	{

		content = 0;

	}

	Any::Any( const Kiwi::Any& other )
	{

		if( other.content != 0 )
		{
			content = other.content;
		} else
		{
			content = 0;
		}

	}

	Any::~Any()
	{
		SAFE_DELETE( content );
	}

	const std::type_info& Any::TypeInfo() const
	{
		return (content) ? content->TypeInfo() : typeid(void);
	}

	Kiwi::Any& Any::Swap( Kiwi::Any& other )
	{
		std::swap( content, other.content );
		return *this;
	}

	Kiwi::Any& Any::operator=( const Kiwi::Any& other )
	{
		return this->Swap( Kiwi::Any( other ) );
	}

	Any::operator const void*() const
	{
		return content;
	}

}