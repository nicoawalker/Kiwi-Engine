#include "Text.h"
#include "Font.h"
#include "Mesh.h"

#include "..\Core\Scene.h"
#include "../Core/Utilities.h"
#include "..\Core\EngineRoot.h"

namespace Kiwi
{

	Text::Text( std::wstring name, Kiwi::Scene& scene, std::wstring font, const Kiwi::Vector2d& maxDimensions ):
		Kiwi::Component( name, L"_text", scene )
	{
		m_font = 0;
		m_fontTexture = 0;
		m_fontName = font;
		m_textAlignment = TEXT_ALIGNMENT::ALIGN_LEFT;
		m_maxCharacters = -1; //no maximum
		m_textColor = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );
		m_maxDimensions = maxDimensions;
		m_textMesh = 0;
	}

	Text::~Text()
	{
	}

	void Text::_OnShutdown()
	{
	}

	void Text::_OnStart()
	{
		if( m_entity != 0 )
		{
			this->FetchAsset( L"Font", m_fontName );
			this->_InitializeText();
		}
	}

	bool Text::_InitializeText()
	{
		if( m_font != 0 )
		{
			m_fontTexture = _kEngine.GetAssetManager().FindAsset<Kiwi::Texture>( L"Texture", m_font->GetTexture() );

			std::unique_ptr<Kiwi::Component> newMesh = m_scene->CreateComponent( m_objectName + L"/mesh", L"_mesh" );
			m_textMesh = dynamic_cast<Kiwi::Mesh*>(newMesh.get());
			if( m_textMesh != 0 )
			{
				m_entity->AttachComponent( std::move( newMesh ) );
				this->_GenerateTextMesh( m_text );
				return true;
			}
		}

		return false;
	}

	Kiwi::EventResponse Text::_OnAssetFetched( Kiwi::IAsset* asset )
	{
		if( asset->GetAssetName().compare( m_fontName ) == 0 )
		{
			m_font = dynamic_cast<Kiwi::Font*>(asset);
			if( this->_InitializeText() && m_fontTexture != 0 )
			{
				return Kiwi::EventResponse::DISCONNECT_THIS;
			}

		} else if( m_font != 0 && asset->GetAssetName().compare( m_font->GetTexture() ) == 0 )
		{
			m_fontTexture = dynamic_cast<Kiwi::Texture*>(asset);
			if( m_fontTexture != 0 )
			{
				return Kiwi::EventResponse::DISCONNECT_THIS;
			}
		}

		return Kiwi::EventResponse::NONE;
	}

	void Text::_GenerateTextMesh( std::wstring text )
	{
		if( m_entity != 0 && text.size() > 0 && m_font != 0 )
		{
			//if( m_textMesh == 0 )
			//{
			//	std::unique_ptr<Kiwi::Component> newMesh = m_scene->CreateComponent( L"mesh", L"_mesh" );
			//	m_textMesh = dynamic_cast<Kiwi::Mesh*>(newMesh.get());
			//	if( m_textMesh != 0 )
			//	{
			//		m_entity->AttachComponent( std::move( newMesh ) );
			//		this->_GenerateTextMesh( m_text );
			//	}
			//}

			//double x = 0.0;
			//double y = 0.0;
			//double width = m_font->CalculateWidth( text );
			//double height = m_font->GetCharacterHeight();

			//if( width > m_maxDimensions.x )
			//{
			//	std::wstring temp;
			//	double totalWidth = 0.0;
			//	double totalHeight = height;
			//	for( unsigned int charIndex = 0; charIndex < text.size(); charIndex++ )
			//	{
			//		if( text[charIndex] == L'\n' )
			//		{
			//			if( charIndex + 1 < text.size() )
			//			{
			//				temp += text[charIndex];
			//				totalHeight += height;
			//			}
			//			continue;
			//		}

			//		//std::wstring character = L"" + text[charIndex];
			//		float charWidth = m_font->GetCharacterWidth( text[charIndex] );
			//		totalWidth += charWidth;
			//		if( totalWidth > m_maxDimensions.x )
			//		{
			//			if( totalHeight + height <= m_maxDimensions.y )
			//			{//insert a new line
			//				totalWidth = 0.0;
			//				temp += L'\n';
			//				temp += text[charIndex];
			//				totalHeight += height;
			//				continue;

			//			} else
			//			{//exceeding max height, dont need any more characters
			//				break;
			//			}

			//		}
			//		temp += text[charIndex];
			//	}

			//	text = temp;
			//	width = m_maxDimensions.x;
			//}

			//m_dimensions.x = width;
			//m_dimensions.y = height;

			//switch( m_textAlignment )
			//{
			//	case TEXT_ALIGNMENT::ALIGN_LEFT:
			//		{
			//			//left-most edge of text will be at an x position of 0
			//			x = 0.0;
			//			y = height / 2.0;
			//			break;
			//		}
			//	case TEXT_ALIGNMENT::ALIGN_CENTRE:
			//		{
			//			//text will be centered around the origin
			//			x = -width / 2.0;
			//			y = height / 2.0;
			//			break;
			//		}
			//	case TEXT_ALIGNMENT::ALIGN_RIGHT:
			//		{
			//			//right-most edge of text will be at an x position of 0
			//			x = -width;
			//			y = height / 2.0;
			//			break;
			//		}
			//	default: return;
			//}

			//std::vector<Kiwi::Vector3d> vertices;
			//std::vector<Kiwi::Vector2d> uvs;
			//std::vector<Kiwi::Vector3d> normals;
			//std::vector<Kiwi::Color> colors;

			//double initialX = x;

			////for each letter, create a quad in the right position and of the right size
			//for( unsigned int i = 0; i < text.length(); i++ )
			//{

			//	if( text[i] == '\n' )
			//	{
			//		y -= height;
			//		x = initialX;
			//		continue;
			//	}

			//	int letter = (int)text[i];

			//	if( letter == 32 )
			//	{
			//		//space, just move over
			//		x += m_font->GetCharacter( 32 ).charWidth;

			//	}else
			//	{
			//		Kiwi::Font::Character character = m_font->GetCharacter( letter );

			//		//top left
			//		vertices.push_back( Kiwi::Vector3d( x, y, 0.0 ) );
			//		uvs.push_back( Kiwi::Vector2d( character.uvLeft, character.uvTop ) );
			//		normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
			//		colors.push_back( m_textColor );

			//		//top right
			//		vertices.push_back( Kiwi::Vector3d( x + character.charWidth, y, 0.0 ) );
			//		uvs.push_back( Kiwi::Vector2d( character.uvRight, character.uvTop ) );
			//		normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
			//		colors.push_back( m_textColor );

			//		//bottom left
			//		vertices.push_back( Kiwi::Vector3d( x, y - character.charHeight, 0.0 ) );
			//		uvs.push_back( Kiwi::Vector2d( character.uvLeft, character.uvBottom ) );
			//		normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
			//		colors.push_back( m_textColor );

			//		//bottom left
			//		vertices.push_back( Kiwi::Vector3d( x, y - character.charHeight, 0.0 ) );
			//		uvs.push_back( Kiwi::Vector2d( character.uvLeft, character.uvBottom ) );
			//		normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
			//		colors.push_back( m_textColor );

			//		//top right
			//		vertices.push_back( Kiwi::Vector3d( x + character.charWidth, y, 0.0 ) );
			//		uvs.push_back( Kiwi::Vector2d( character.uvRight, character.uvTop ) );
			//		normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
			//		colors.push_back( m_textColor );

			//		//bottom right
			//		vertices.push_back( Kiwi::Vector3d( x + character.charWidth, y - character.charHeight, 0.0 ) );
			//		uvs.push_back( Kiwi::Vector2d( character.uvRight, character.uvBottom ) );
			//		normals.push_back( Kiwi::Vector3d( 0.0, 0.0, -1.0 ) );
			//		colors.push_back( m_textColor );
			//		
			//		x += (double)(character.charWidth + m_font->GetCharacterSpacing());
			//	}
			//}

			//m_textMesh->Clear();
			//m_textMesh->SetVertices( vertices );
			//m_textMesh->SetUVs( uvs );
			//m_textMesh->SetNormals( normals );
			//m_textMesh->SetColors( colors );

			//Kiwi::Submesh submesh;
			//submesh.startIndex = 0;
			//submesh.endIndex = (unsigned int)vertices.size();
			//submesh.material.SetTexture( Material::TEXTURE_TYPE::DIFFUSE, m_fontTexture );
			//submesh.material.SetColor( Material::COLOR_TYPE::DIFFUSE, m_textColor );
			//m_textMesh->AddSubmesh( submesh );

			//m_textMesh->Commit();
			//m_textMesh->SetShader( m_shaderName );

			////release the memory used by vertices
			//FreeMemory( vertices );
			//FreeMemory( uvs );
			//FreeMemory( normals );
		}
	}

	void Text::SetText( std::wstring text )
	{
		if( m_maxCharacters != -1 && text.length() > m_maxCharacters )
		{
			text = text.substr( 0, m_maxCharacters );
		}

		this->_GenerateTextMesh( text );

		m_text = text;
	}

	void Text::SetAlignment( TEXT_ALIGNMENT alignment )
	{
		if( alignment != m_textAlignment )
		{
			m_textAlignment = alignment;
			this->_GenerateTextMesh( m_text );
		}
	}

	void Text::SetColor( const Kiwi::Color& color )
	{
		if( m_textMesh )
		{
			/*if( m_textMesh->GetSubmeshCount() > 0 )
			{
				Kiwi::Submesh* submesh = m_textMesh->GetSubmesh( 0 );
				if( submesh )
				{
					submesh->material.SetColor( Material::COLOR_TYPE::DIFFUSE, color );
				}
			}*/
		}
		m_textColor = color;
	}

	void Text::SetShader( std::wstring shaderName )
	{
		/*if( m_textMesh != 0 )
		{
			m_textMesh->SetShader( shaderName );
		}*/

		m_shaderName = shaderName;
	}

	double Text::GetCharacterHeight()const
	{
		if( m_font != 0 )
		{
			return 0.0;// (double)m_font->GetCharacterHeight();

		} else
		{
			return 0.0;
		}
	}

};