#ifndef _KIWI_EVENT_H_
#define _KIWI_EVENT_H_

namespace Kiwi
{

	template<class SourceType>
	class Event
	{
	protected:

		SourceType* m_source;

	public:

		Event(SourceType* source):
			m_source(source){}
		virtual ~Event() {}

		SourceType* GetSource()const { return m_source; }

	};
};

#endif