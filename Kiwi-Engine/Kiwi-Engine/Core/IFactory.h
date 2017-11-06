#ifndef _KIWI_IFACTORY_H_
#define _KIWI_IFACTORY_H_

#include <string>

namespace Kiwi
{
	class IFactory
	{
	public:

		virtual ~IFactory() {}

		/*returns a new instance of the correct derived type of this factory*/
		virtual IFactory* Clone()const = 0;

		/*returns a string identifying the type of object the factory creates*/
		virtual std::wstring GetObjectType()const = 0;

	};
}

#endif