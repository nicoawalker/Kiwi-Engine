#include "Text.h"
#include "Font.h"
#include "Mesh.h"
#include "IRenderable.h"

#include "..\Core\Scene.h"
#include "..\Core\Utilities.h"

namespace Kiwi
{

	Text::Text( std::wstring name, Kiwi::Scene* scene, std::wstring font, unsigned int maxCharacters ):
		Kiwi::Entity(name, scene)
	{

		assert( m_scene != 0 );

		m_font = m_scene->FindAssetWithName<Kiwi::Font>(font);
		assert( m_font != 0 );

		m_entityType = ENTITY_2D;
		m_maxCharacters = maxCharacters;
		m_textColor = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );

		Kiwi::Mesh textMesh( name + L"/Mesh", L"", m_scene->GetRenderer() );
		m_renderable = new Kiwi::IRenderable( name + L"/Renderable", this, textMesh, L"Default2DShader" );

	}

	Text::~Text()
	{
	}

	void Text::_GenerateTextMesh( std::wstring text, Font::ALIGNMENT textAlignment )
	{

		if( m_renderable == 0 )
		{
			return;
		}

		Kiwi::Mesh* mesh = m_renderable->GetMesh();

		float x = 0.0f;
		float y = 0.0f;
		float width = m_font->CalculateWidth( text );
		float height = m_font->GetCharacterHeight();

		m_dimensions.x = width;
		m_dimensions.y = height;

		switch( textAlignment )
		{
			case Font::ALIGN_LEFT:
				{
					//left-most edge of text will be at an x position of 0
					x = 0.0f;
					y = height / 2.0f;
					break;
				}
			case Font::ALIGN_CENTRE:
				{
					//text will be centered around the origin
					x = -width / 2.0f;
					y = height / 2.0f;
					break;
				}
			case Font::ALIGN_RIGHT:
				{
					//right-most edge of text will be at an x position of 0
					x = -width;
					y = height / 2.0f;
					break;
				}
			default: return;
		}

		std::vector<Kiwi::Mesh::Subset> meshSubsets( 1 );
		meshSubsets[0].material.SetTexture( L"Diffuse", m_font->GetTexture() );


		//for each letter, create a quad in the right position and of the right size
		for( unsigned int i = 0; i < text.length(); i++ )
		{
			int letter = (int)text[i];

			if( letter == 32 )
			{
				//space, just move over
				x += m_font->GetCharacter( 32 ).charWidth;

			} else
			{
				Kiwi::Font::Character character = m_font->GetCharacter( letter );

				//top left
				Kiwi::Mesh::Vertex v1;
				v1.position.x = x;
				v1.position.y = y;
				v1.position.z = 0.0f;
				v1.textureUV.x = character.left;
				v1.textureUV.y = character.top;
				v1.normals.x = 0.0f;
				v1.normals.y = 0.0f;
				v1.normals.z = 1.0f;
				meshSubsets[0].vertices.push_back( v1 );

				//top right
				Kiwi::Mesh::Vertex v2;
				v2.position.x = x + character.charWidth;
				v2.position.y = y;
				v2.position.z = 0.0f;
				v2.textureUV.x = character.right;
				v2.textureUV.y = character.top;
				v2.normals.x = 0.0f;
				v2.normals.y = 0.0f;
				v2.normals.z = 1.0f;
				meshSubsets[0].vertices.push_back( v2 );

				//bottom left
				Kiwi::Mesh::Vertex v3;
				v3.position.x = x;
				v3.position.y = y - character.charHeight;
				v3.position.z = 0.0f;
				v3.textureUV.x = character.left;
				v3.textureUV.y = character.bottom;
				v3.normals.x = 0.0f;
				v3.normals.y = 0.0f;
				v3.normals.z = 1.0f;
				meshSubsets[0].vertices.push_back( v3 );

				//bottom left
				meshSubsets[0].vertices.push_back( v3 );

				//top right
				meshSubsets[0].vertices.push_back( v2 );

				//bottom right
				Kiwi::Mesh::Vertex v4;
				v4.position.x = x + character.charWidth;
				v4.position.y = y - character.charHeight;
				v4.position.z = 0.0f;
				v4.textureUV.x = character.right;
				v4.textureUV.y = character.bottom;
				v4.normals.x = 0.0f;
				v4.normals.y = 0.0f;
				v4.normals.z = 1.0f;
				meshSubsets[0].vertices.push_back( v4 );

				x += character.charWidth + m_font->GetCharacterSpacing();

			}

		}

		mesh->SetData( meshSubsets );

		//release the memory used by vertices
		FreeMemory( meshSubsets );

	}

	void Text::SetText( std::wstring text, Font::ALIGNMENT textAlignment )
	{

		if( text.length() > m_maxCharacters )
		{
			text = text.substr( 0, m_maxCharacters );
		}

		this->_GenerateTextMesh( text, textAlignment );

		m_text = text;

	}

	void Text::SetColor( const Kiwi::Color& color )
	{

		if( m_renderable )
		{
			m_renderable->GetMesh()->GetSubsetMaterial( 0 )->SetColor( L"Diffuse", color );
		}

	}

};