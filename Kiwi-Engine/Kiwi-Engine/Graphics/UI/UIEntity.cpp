#include "UIEntity.h"
#include "UserInterface.h"

#include "../Renderable2D.h"

namespace Kiwi
{

	UIEntity::UIEntity( std::wstring name, Kiwi::UserInterface& parentInterface ):
		Kiwi::Entity( name, parentInterface.GetScene() )
	{

		m_parentInterface = &parentInterface;
		m_entityType = ENTITY_2D;

	}

	UIEntity::~UIEntity()
	{
	}

}