#ifndef _KIWI_IEVENT_H_
#define _KIWI_IEVENT_H_

#include "../Types.h"

#include <string>

namespace Kiwi
{

	template<typename SourceType>
	class IEvent
	{
	public:

		SourceType* source;

		std::wstring eventCategory;

		Kiwi::StringPair parameters;

	public:

		IEvent( SourceType* eventSource, const std::wstring& category ):
			source( eventSource ), eventCategory( category )
		{ 
		}

		IEvent( SourceType* eventSource, const std::wstring& category, const Kiwi::StringPair& params ) :
			source( eventSource ), eventCategory( category ), parameters(params)
		{
		}

		virtual ~IEvent() {}

	};
}

#endif