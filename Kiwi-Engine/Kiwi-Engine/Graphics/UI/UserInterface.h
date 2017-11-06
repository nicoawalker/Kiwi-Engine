#ifndef _KIWI_USERINTERFACE_H_
#define _KIWI_USERINTERFACE_H_

#include "../../Core/Entity.h"
#include "../../Core/Vector2.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Kiwi
{

	class Scene;
	class UIEntity;

	class UserInterface:
		public Kiwi::Entity
	{
	friend class Scene;
	protected:

		std::unordered_map<int, UIEntity*> m_uiEntities;

	public:

		UserInterface( std::wstring name, Kiwi::Scene& scene );
		virtual ~UserInterface();

		virtual void Update() {}
		virtual void FixedUpdate();
		virtual void Shutdown();

	};
}

#endif