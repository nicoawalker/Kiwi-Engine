#ifndef _KIWI_IOBJECT_H_
#define _KIWI_IOBJECT_H_

#include "Utilities.h"

#include <string>

namespace Kiwi
{
	class IObject
	{
	protected:

		std::wstring m_name;

		long long m_uid;

	public:

		IObject( const std::wstring& name ) :
			m_name( name ), m_uid( Kiwi::NewUID() )
		{
		}

		IObject( const Kiwi::IObject& obj ) :
			m_name( obj.GetName() ), m_uid( Kiwi::NewUID() )
		{
		}

		virtual ~IObject() = 0;

		const std::wstring& GetName()const { return m_name; }

		long long GetUID()const { return m_uid; }

	};

	Kiwi::IObject::~IObject() {}

}

#endif