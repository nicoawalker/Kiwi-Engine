#include "UISprite.h"
#include "SpriteAnimator.h"
#include "UICanvas.h"

#include "..\Texture.h"
#include "../Mesh.h"

#include "../../Core/Scene.h"
#include "..\..\Core\Exception.h"
#include "../../Core/EngineRoot.h"

namespace Kiwi
{

	UISprite::UISprite( const std::wstring& name, Kiwi::UICanvas& canvas, const std::wstring& textureName ) :
		Kiwi::IUIWidget( name, canvas, L"UISprite" )
	{
		m_textureName = textureName;

		Kiwi::Mesh* mesh =  this->AttachComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
		mesh->SetModel( L"Primitive_Quad" );
		mesh->SetRenderGroup( m_widgetCanvas->GetName() );
		//this->AttachComponent( std::make_unique<Kiwi::SpriteAnimator>( name + L"/SpriteAnimator", *m_widgetCanvas->GetScene(), m_textureName ) );
	}

	UISprite::~UISprite()
	{
	}

	void UISprite::_OnStart()
	{
		Kiwi::Mesh* mesh = this->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
		if( mesh == nullptr )
		{
			Kiwi::Mesh* mesh = this->AttachComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
			mesh->SetModel( L"Primitive_Quad" );
		}

		if( mesh != nullptr )
		{
			Kiwi::IModel::Subset* subset = mesh->GetModel()->GetSubset( 0 );
			if( subset != nullptr )
			{
				subset->GetMaterial()->SetTexture( Material::TEXTURE_TYPE::DIFFUSE, m_textureName );
				subset->GetMaterial()->SetShader( m_widgetCanvas->GetWidgetShader() );
			}
		}

		m_widgetCanvas->_NotifyWidgetResized( *this );
		m_widgetCanvas->_NotifyWidgetMoved( *this );
	}

	void UISprite::_OnShutdown()
	{
	}

	void UISprite::SetTexture( const std::wstring& textureName )
	{
		/*m_textureName = textureName;
		if( m_mesh != nullptr )
		{
			Kiwi::Submesh* submesh = m_mesh->GetSubmesh( 0 );
			if( submesh != 0 )
			{
				submesh->material.SetTexture( Material::TEXTURE_TYPE::DIFFUSE, textureName );
			}
		}*/
	}

	void UISprite::SetTextureUVs( const std::vector<Kiwi::Vector2d>& uvs )
	{
		/*if( m_mesh != nullptr )
		{
			m_mesh->SetUVs( uvs );
			m_mesh->Commit();
		}*/
	}

	void UISprite::SetColor( const Kiwi::Color& color )
	{
		/*if( m_mesh != nullptr )
		{
			Kiwi::Submesh* submesh = m_mesh->GetSubmesh( 0 );
			if( submesh != 0 )
			{
				submesh->material.SetColor( Material::COLOR_TYPE::DIFFUSE, color );
			}
		}*/
	}

}