#ifndef _KIWI_UIENTITY_H_
#define _KIWI_UIENTITY_H_

#include "UserInterface.h"

#include "../../Core/Entity.h"
#include "../../Core/Vector3d.h"

#include <string>
#include <unordered_map>

namespace Kiwi
{

	class UIEntity:
		public Kiwi::Entity
	{
	protected:

		UserInterface* m_parentInterface;

	public:

		UIEntity( std::wstring name, Kiwi::UserInterface& parentInterface );
		~UIEntity();

		UserInterface& GetInterface() { return *m_parentInterface; }

	};
}

#endif