#include "UITextbox.h"
#include "UICanvas.h"
#include "UISprite.h"

#include "..\RenderTarget.h"
#include "../Font.h"
#include "..\Mesh.h"
#include "../StaticModel.h"

#include "../../Core/Scene.h"
#include "..\..\Core\EngineRoot.h"

namespace Kiwi
{

	UITextbox::UITextbox( const std::wstring& name, Kiwi::UICanvas& canvas, const std::wstring& fontName, const std::wstring& textboxTexture, const std::wstring& text ) :
		Kiwi::IUIWidget( name, canvas, L"UITextbox" )
	{
		m_fontName = fontName;
		m_textboxTextureName = textboxTexture;
		m_text = text;

		m_font = nullptr;
		m_textAlignment = TEXT_ALIGNMENT::ALIGN_LEFT;
		m_textColor.Set( 1.0, 1.0, 1.0, 1.0 );
		m_rebuildTextMesh = false;
		m_wordWrapEnabled = true;
		m_textSharpening = 0.1;
		m_lineSpacing = 0.0;

		m_entityType = Kiwi::Entity::ENTITY_2D;

		this->_CreateMesh();
	}

	Kiwi::Mesh* UITextbox::_CreateMesh()
	{
		Kiwi::Mesh* mesh = this->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
		if( mesh != nullptr && mesh->GetModel() != nullptr ) return nullptr;

		if( mesh == nullptr )
		{
			mesh = this->AttachComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
			if( mesh == nullptr ) return nullptr;
		}

		mesh->SetRenderGroup( m_widgetCanvas->GetName() );
		mesh->SetRenderTarget( m_widgetCanvas->GetName() + L"/RenderTarget" );

		Kiwi::StaticModel* model = _kAssetManager.CreateModel( m_objectName + L"/Mesh/Model" );
		if( model == nullptr ) return nullptr;

		/*create a new material for the model to use*/
		Kiwi::Material* modelMaterial = _kAssetManager.CreateMaterial( m_objectName + L"/Mesh/Model/Material" );
		if( modelMaterial == nullptr ) return nullptr;
		modelMaterial->Reserve();
		modelMaterial->SetShader( m_widgetCanvas->GetWidgetShader() );

		mesh->SetModel( model, false );

		return mesh;
	}

	void UITextbox::_OnShutdown()
	{
		/*release the textbox model's material*/
		Kiwi::Material* material = _kAssetManager.FindAsset<Kiwi::Material>( L"Model", m_objectName + L"/Mesh/Model/Material" );
		if( material != nullptr ) material->Free();
	}

	void UITextbox::_OnStart()
	{
		/*get the font asset that the textbox will use*/
		this->FetchAsset( L"Font", m_fontName );

		/*create a sprite for the textbox background image and attach it as a child*/
		if( m_textboxTextureName.length() > 0 )
		{
			Kiwi::UISprite* textboxSprite = m_widgetCanvas->CreateSprite( m_objectName + L"/sprite", m_textboxTextureName );
			if( textboxSprite != nullptr )
			{
				this->AttachChild( textboxSprite );
				textboxSprite->SetWidgetDimensions( m_textboxDimensions );
				textboxSprite->SetWidgetDepth( -0.01 );
			}
		}

		/*if there is text set, schedule a rebuild of the mesh for the next update*/
		if( m_text.length() > 0 ) m_rebuildTextMesh = true;

		m_widgetCanvas->_NotifyWidgetResized( *this );
		m_widgetCanvas->_NotifyWidgetMoved( *this );
	}

	void UITextbox::_OnUpdate()
	{
		if( m_rebuildTextMesh )
		{
			this->_GenerateTextMesh();
			m_rebuildTextMesh = false;
		}
	}

	Kiwi::EventResponse UITextbox::_OnAssetFetched( Kiwi::IAsset* asset )
	{
		if( asset->GetAssetName().compare( m_fontName ) == 0 )
		{
			m_font = dynamic_cast<Kiwi::Font*>(asset);
			if( m_font != nullptr )
			{
				if( m_text.length() > 0 )m_rebuildTextMesh = true;

				return Kiwi::EventResponse::DISCONNECT_THIS;
			}
		}

		return Kiwi::EventResponse::NONE;
	}

	void UITextbox::_GenerateTextMesh()
	{
		if( m_text.length() == 0 || m_font == nullptr ) return;

		Kiwi::Mesh* mesh = this->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );

		/*if the textbox doesn't have a mesh yet, create it now*/
		if( mesh == nullptr )
		{
			mesh = this->_CreateMesh();
			if( mesh == nullptr ) return;
		}

		double x = 0.0;
		double y = 0.0;
		double width = m_font->CalculateWidth( m_text );
		double height = m_font->GetLineHeight();
		std::wstring outputString; //final string that will be output

		/*apply word wrapping if it's enabled, otherwise just trim the output string*/
		if( width > m_textboxDimensions.x - (m_textboxMargins.x + m_textboxMargins.z) )
		{
			double totalWidth = 0.0;
			double totalHeight = height;
			uint lastSpaceIndex = 0;
			for( uint charIndex = 0; charIndex < (uint)m_text.length(); charIndex++ )
			{
				if( m_text[charIndex] == L'\n' )
				{
					/*add the \n character and adjust line height only if there are more characters after the \n and there is room for another line*/
					if( charIndex + 1 < (uint)m_text.length() && totalHeight + height + m_lineSpacing <= m_textboxDimensions.y - (m_textboxMargins.y + m_textboxMargins.w) )
					{
						outputString += '\n'; //add newline
						totalHeight += height + m_lineSpacing; //increase totalheight by the height of the newline and any line spacing
						lastSpaceIndex = (uint)outputString.length() - 1; //set the last space index to the start of this new line
						continue;

					} else
					{//can't fit another line in, we're done
						break;
					}
				}

				double charWidth = m_font->GetCharacter( m_text[charIndex] ).offset.x + m_font->GetCharacter( m_text[charIndex] ).dimensions.x;
				
				if( totalWidth + charWidth > m_textboxDimensions.x - (m_textboxMargins.x + m_textboxMargins.z) )
				{
					if( totalHeight + height + m_lineSpacing <= m_textboxDimensions.y - (m_textboxMargins.y + m_textboxMargins.w) )
					{//insert a new line before the current word
						totalWidth = 0.0;
						if( m_text[charIndex] == L' ' )
						{/*replace the space with a newline and continue*/
							outputString += L'\n';
							lastSpaceIndex = (uint)outputString.length() - 1; //set the last space index to the start of this new line

						}else if( lastSpaceIndex > 0 )
						{/*remove all characters since the last space, including the space, insert a newline character and continue*/
							std::wstring temp = outputString.substr( 0, lastSpaceIndex ); //get a substring of everything up to, but not including, the last space
							charIndex -= uint( outputString.length() - temp.length() ); //go back to just after the last space
							outputString = temp;
							outputString += L'\n';
							lastSpaceIndex = (uint)outputString.length() - 1; //set the last space index to the start of this new line

						} else
						{/*no complete last word found, just insert a newline character and continue*/
							outputString += m_text[charIndex];
							outputString += L'\n';
							lastSpaceIndex = (uint)outputString.length() - 1; //set the last space index to the start of this new line
						}

						totalHeight += height + m_lineSpacing; //increase totalheight by the height of the newline and any line spacing

						continue;

					} else
					{//exceeding max height, dont need any more characters
						break;
					}
				}
				totalWidth += m_font->GetCharacter( m_text[charIndex] ).xAdvance;
				outputString += m_text[charIndex];

				if( m_text[charIndex] == L' ' )
				{
					lastSpaceIndex = (uint)outputString.length() - 1; //set the last space index to the start of this new line
				}
			}

			width = m_textboxDimensions.x - (m_textboxMargins.x + m_textboxMargins.z);

		} else
		{
			outputString = m_text;
		}

		m_textDimensions.x = width;
		m_textDimensions.y = height;

		switch( m_textAlignment )
		{
			case TEXT_ALIGNMENT::ALIGN_LEFT:
				{
					x = -(m_textboxDimensions.x / 2.0) + m_textboxMargins.x;//left-most edge of text will align with left edge of textbox
					y = (m_textboxDimensions.y / 2.0) - m_textboxMargins.y; //text starts at top edge of textbox
					break;
				}
			case TEXT_ALIGNMENT::ALIGN_CENTRE:
				{
					x = -width / 2.0;//text will be centered around the origin
					y = (m_textboxDimensions.y / 2.0) - m_textboxMargins.y;//text starts at top edge of textbox
					break;
				}
			case TEXT_ALIGNMENT::ALIGN_RIGHT:
				{
					x = ((m_textboxDimensions.x / 2.0) - width) - m_textboxMargins.z;//right-most edge of text will align with right edge of textbox
					y = (m_textboxDimensions.y / 2.0) - m_textboxMargins.y;//text starts at top edge of textbox
					break;
				}
			default: return;
		}

		std::vector<Kiwi::Vertex> vertices;

		double initialX = x;

		//for each letter, create a quad in the right position and of the right size
		for( unsigned int i = 0; i < outputString.length(); i++ )
		{
			if( outputString[i] == '\n' )
			{
				y -= height + m_lineSpacing;
				x = initialX;
				continue;
			}

			Kiwi::Font::Character character = m_font->GetCharacter( outputString[i] );

			if( x == initialX )
			{/*shift the first character over so that the left edge is at and x position of 0*/
				x -= character.offset.x;
			}

			if( outputString[i] != 32 ) //for all characters except space
			{
				//top left
				vertices.push_back( Kiwi::Vertex( Kiwi::Vector3d( x + character.offset.x, y - character.offset.y, 0.0 ), //position
												  Kiwi::Vector2d( character.uvs.x, character.uvs.y ), //texture UVs
												  Kiwi::Vector3d( 0.0, 0.0, -1.0 ), //normal
												  m_textColor ) ); //color

				//top right
				vertices.push_back( Kiwi::Vertex( Kiwi::Vector3d( x + character.dimensions.x + character.offset.x, y - character.offset.y, 0.0 ),
												  Kiwi::Vector2d( character.uvs.z, character.uvs.y ),
												  Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
												  m_textColor ) );

				//bottom left
				vertices.push_back( Kiwi::Vertex( Kiwi::Vector3d( x + character.offset.x, y - (character.dimensions.y + character.offset.y), 0.0 ),
												  Kiwi::Vector2d( character.uvs.x, character.uvs.w ),
												  Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
												  m_textColor ) );

				//bottom left
				vertices.push_back( Kiwi::Vertex( Kiwi::Vector3d( x + character.offset.x, y - (character.dimensions.y + character.offset.y), 0.0 ),
												  Kiwi::Vector2d( character.uvs.x, character.uvs.w ),
												  Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
												  m_textColor ) );

				//top right
				vertices.push_back( Kiwi::Vertex( Kiwi::Vector3d( x + character.dimensions.x + character.offset.x, y - character.offset.y, 0.0 ),
												  Kiwi::Vector2d( character.uvs.z, character.uvs.y ),
												  Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
												  m_textColor ) );

				//bottom right
				vertices.push_back( Kiwi::Vertex( Kiwi::Vector3d( x + character.dimensions.x + character.offset.x, y - (character.dimensions.y + character.offset.y), 0.0 ),
												  Kiwi::Vector2d( character.uvs.z, character.uvs.w ),
												  Kiwi::Vector3d( 0.0, 0.0, -1.0 ),
												  m_textColor ) );
			}

			x += character.xAdvance + m_font->GetCharacterSpacing();
		}

		Kiwi::StaticModel* model = dynamic_cast<Kiwi::StaticModel*>(mesh->GetModel());
		if( model == nullptr ) return;

		//model->Clear();

		model->SetVertices( vertices );

		/*create indices for each vertex*/
		std::vector<unsigned long> indices;
		for( unsigned long i = 0; i < (unsigned long)vertices.size(); i++ )
		{
			indices.push_back( i );
		}
		model->SetIndices( indices );

		/*create the main subset*/
		Kiwi::IModel::Subset subset( m_objectName + L"/Mesh/Model/Material", 0, (unsigned int)vertices.size(), 0, (unsigned int)vertices.size(), 0, 0 );
		if( subset.GetMaterial() != nullptr )
		{
			subset.GetMaterial()->SetTexture( Material::TEXTURE_TYPE::DIFFUSE, m_font->GetTexture() );
			subset.GetMaterial()->SetColor( Material::COLOR_TYPE::DIFFUSE, m_textColor );
		}

		model->SetSubsets( std::vector<Kiwi::IModel::Subset> {subset} );
			
		model->CommitChanges();

		//KIWIDEBUGSTRING( Kiwi::ToWString( L"\nText vertices: " + Kiwi::ToWString( vertices.size() ) ).c_str() );

		m_widgetCanvas->_NotifyWidgetChanged( *this );
	}

	void UITextbox::_OnWidgetMoved( const Kiwi::Vector3d& translation )
	{
		//if( translation.z != 0.0 )
		//{
		//	/*if the depth has changed, need to change the sprite depth to still be under the text*/
		//	Kiwi::UISprite* textboxSprite = this->FindComponent<Kiwi::UISprite>( m_objectName + L"/Sprite" );
		//	if( textboxSprite != nullptr )
		//	{
		//		textboxSprite->SetWidgetDepth( m_widgetPosition.z - 0.1 );
		//	}
		//}
	}

	void UITextbox::_OnWidgetResized( const Kiwi::Vector2d& resizeAmount )
	{
		/*if there is a textbox sprite, resize it as well*/
		//Kiwi::UISprite* textboxSprite = this->FindComponent<Kiwi::UISprite>( m_objectName + L"/sprite" );
		//if( textboxSprite != nullptr )
		//{
		//	textboxSprite->SetWidgetDimensions( m_widgetDimensions );
		//}

		//m_textboxDimensions = m_widgetDimensions;

		//m_widgetDimensions.Set( 1.0, 1.0 );

		///*the maximum size has changed so we need to re-create the mesh to fit*/
		//if( m_text.length() > 0 )m_rebuildTextMesh = true;
	}

	void UITextbox::SetLineSpacing( double lineSpacing )
	{
		if( lineSpacing != m_lineSpacing )
		{
			m_lineSpacing = lineSpacing;
			if( m_text.length() > 0 )m_rebuildTextMesh = true;
		}
	}

	void UITextbox::SetText( const std::wstring& newText )
	{
		m_text = newText;

		if( m_text.length() > 0 )m_rebuildTextMesh = true;
	}

	void UITextbox::AppendText( const std::wstring& newText )
	{
		m_text.append( newText );

		if( m_text.length() > 0 )m_rebuildTextMesh = true;
	}

	void UITextbox::SetTextboxMargins( const Kiwi::Vector4d& margins )
	{
		if( margins != m_textboxMargins )
		{
			m_textboxMargins = margins;
			if( m_text.length() > 0 )m_rebuildTextMesh = true;
		}
	}

	void UITextbox::SetTextColor( const Kiwi::Color& color )
	{
		if( color != m_textColor )
		{
			m_textColor = color;
			if( m_text.length() > 0 )m_rebuildTextMesh = true;
		}
	}

}