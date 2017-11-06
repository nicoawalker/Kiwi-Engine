#include "UserInterface.h"
#include "UIEntity.h"

#include "../Renderer.h"
#include "../RenderWindow.h"

#include "../../Core/Scene.h"
#include "../../Core/Assert.h"
#include "../../Core/Utilities.h"

namespace Kiwi
{

	UserInterface::UserInterface( std::wstring name, Kiwi::Scene& scene ):
		Kiwi::Entity( name, scene )
	{

		m_position = Kiwi::Vector2( 0.0f, 0.0f );

		//get the width and height of the window
		Kiwi::Renderer* renderer = m_scene->GetRenderer();
		assert( renderer != 0 );
		Kiwi::RenderWindow* rWindow = renderer->GetRenderWindow();
		assert( rWindow != 0 );

		m_dimensions = rWindow->GetClientSize();
		m_windowDimensions = rWindow->GetClientSize();

	}

	UserInterface::~UserInterface()
	{

		Kiwi::FreeMemory( m_components );

	}

	void UserInterface::FixedUpdate()
	{

		//go through all of the components and clamp their positions within the interface bounds
		//auto itr = m_components.begin();
		//for( ; itr != m_components.end(); itr++ )
		//{
		//	if( itr->second && itr->second->IsShutdown() == false )
		//	{
		//		Kiwi::Vector3 pos = itr->second->GetTransform()->GetPosition();
		//		Kiwi::Vector3 scale = itr->second->GetTransform()->GetScale();
		//		if( (pos.x + (scale.x / 2.0)) > m_position.x + (m_dimensions.x / 2.0) )
		//		{//component is off the right side
		//			itr->second->GetTransform()->SetPosition( Kiwi::Vector3( (m_position.x + (m_dimensions.x / 2.0)) - (scale.x / 2.0), pos.y, pos.z ) );

		//		} else if( (pos.x - (scale.x / 2.0)) < (m_position.x + (m_dimensions.x / 2.0)) )
		//		{//component is off the left side
		//			itr->second->GetTransform()->SetPosition( Kiwi::Vector3( m_position.x, pos.y, pos.z ) );
		//		}

		//		pos = itr->second->GetTransform()->GetPosition();
		//		if( pos.y + (scale.y / 2.0) > m_position.y + (m_dimensions.y / 2.0) )
		//		{//component is off the top
		//			itr->second->GetTransform()->SetPosition( Kiwi::Vector3( pos.x, (m_position.y + (m_dimensions.x / 2.0)) - (scale.y / 2.0), pos.z ) );

		//		} else if( pos.y - (scale.y / 2.0) < m_position.y - (m_dimensions.y / 2.0) )
		//		{//component is off the bottom
		//			itr->second->GetTransform()->SetPosition( Kiwi::Vector3( pos.x, (m_position.y - (m_dimensions.x / 2.0)) + (scale.y / 2.0), pos.z ) );
		//		}
		//	}
		//}

	}

	void UserInterface::Shutdown()
	{

		auto itr = m_components.begin();
		for( ; itr != m_components.end(); )
		{
			if( itr->second )
			{
				itr->second->Shutdown();
			}
			itr = m_components.erase( itr );
		}

		if( m_scene )
		{
			m_scene->SetUserIterface( 0 );
		}

	}

	void UserInterface::AddComponent( Kiwi::UIComponent* component )
	{

		assert( m_scene != 0 );

		if( component )
		{
			m_components[component->GetName()] = component;
			m_scene->AddEntity( component );

			/*if( component->GetComponents().size() > 0 )
			{
				auto itr = component->GetComponents().begin();
				for( ; itr != component->GetComponents().end(); itr++ )
				{
					this->AddComponent( itr->second );
				}
			}*/

		}

	}

}