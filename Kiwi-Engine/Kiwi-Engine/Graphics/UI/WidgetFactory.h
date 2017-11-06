#ifndef _KIWI_WIDGETFACTORY_H_
#define _KIWI_WIDGETFACTORY_H_

/*abstract factory class used to create new instances of widget subclasses*/

#include "IUIWidget.h"

#include "..\..\Types.h"
#include "../../Core/IGameObjectFactory.h"

#include <string>

namespace Kiwi
{

	class UICanvas;

	class WidgetFactory:
		public Kiwi::IGameObjectFactory
	{
	protected:

		std::recursive_mutex m_factoryMutex;

		static std::wstring ObjectType;

	protected:

		virtual std::unique_ptr<Kiwi::GameObject> _CreateObject( const std::wstring& name, Kiwi::Scene& scene, const Kiwi::StringPair& parameters );

	public:

		WidgetFactory() {}
		virtual ~WidgetFactory() {}

		virtual IFactory* Clone()const { return new WidgetFactory(); }

		/*returns a string identifying the type of object the factory creates*/
		virtual std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif