#ifndef _KIWI_SINGLETON_H_
#define _KIWI_SINGLETON_H_

#include "Exception.h"

namespace Kiwi
{

	template<typename T>
	class Singleton
	{
	private:

		static T* m_singleton;

	public:

		Singleton()
		{
			KIWI_ASSERT( !m_singleton, L"Singleton", L"Can't create multiple copies of a singleton", KIWI_EXCEPTION::DUPLICATIONATTEMPT );

			m_singleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			m_singleton = 0;
		}

		static T& GetSingleton()
		{
			KIWI_ASSERT( m_singleton != 0, L"Singleton::GetSingleton", L"Failed to get singleton: pointer was NULL", KIWI_EXCEPTION::NULLOPERATION );

			return *m_singleton;
		}

		static T* GetSingletonPtr()
		{
			return m_singleton;
		}

	};

	template <typename T> T* Singleton<T>::m_singleton = 0;

}

#endif