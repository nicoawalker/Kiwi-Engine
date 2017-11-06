#include "IReferencedObject.h"
#include "Utilities.h"

#include "../Types.h"

#include <Windows.h>

namespace Kiwi
{

	IReferencedObject::~IReferencedObject()
	{
		if( m_referenceCount != 0 )
		{
			KIWIDEBUGSTRING( Kiwi::ToWString( L"WARNING: Referenced object \"" + m_name + L"\" was deleted with reference count " + Kiwi::ToWString( m_referenceCount ) + L"\n" ).c_str() );
		}
	}

	void IReferencedObject::Reserve()
	{
		std::lock_guard<std::mutex> lock( m_referenceMutex );

		m_referenceCount++;
	}

	void IReferencedObject::Free()
	{
		std::lock_guard<std::mutex> lock( m_referenceMutex );

		if( m_referenceCount > 0 )
		{
			m_referenceCount--;
		}
	}

}