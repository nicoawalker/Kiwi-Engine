#ifndef _KIWI_IREFERENCEDOBJECT_H_
#define _KIWI_IREFERENCEDOBJECT_H_

#include "IObject.h"

#include <mutex>
#include <string>

namespace Kiwi
{

	class IReferencedObject:
		public Kiwi::IObject
	{
	protected:

		std::mutex m_referenceMutex;

		/*stores the number of objects that are currently Reserve()ing this object*/
		int m_referenceCount;

	public:

		IReferencedObject( const std::wstring& name) :
			Kiwi::IObject( name ),
			m_referenceCount( 0 )
		{
		}

		IReferencedObject( const IReferencedObject& obj):
			Kiwi::IObject( obj ),
			m_referenceCount( 0 )
		{
		}

		virtual ~IReferencedObject();

		/*increases the reference count of the object
		the reference count must then be decremented at some point by calling Free()*/
		void Reserve();

		/*decreases the reference count*/
		void Free();

		/*returns the current reference count*/
		int GetReferenceCount()const { return m_referenceCount; }

	};
}

#endif