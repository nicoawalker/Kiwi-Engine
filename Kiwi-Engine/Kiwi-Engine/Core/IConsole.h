#ifndef _KIWI_ICONSOLE_H_
#define _KIWI_ICONSOLE_H_

#include <string>

namespace Kiwi
{

	class IConsole
	{
	protected:



	public:

		IConsole() {}
		virtual ~IConsole() = 0;

		virtual void AddMessage( std::wstring message ) {}

		virtual void OnUpdate() {}
		virtual void OnFixedUpdate() {}

	};

}

#endif