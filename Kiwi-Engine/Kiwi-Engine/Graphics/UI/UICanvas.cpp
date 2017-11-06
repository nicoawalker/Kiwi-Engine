#include "UICanvas.h"
#include "UISprite.h"
#include "UIButton.h"
#include "IUIWidget.h"
#include "UITextbox.h"
#include "UIScrollBar.h"

#include "../Mesh.h"
#include "..\StaticModel.h"

#include "..\..\Core\Scene.h"
#include "../../Core/EngineRoot.h"

namespace Kiwi
{

	UICanvas::UICanvas( Kiwi::Scene& scene, std::wstring canvasName, const Kiwi::Vector2d& dimensions, Kiwi::Camera* camera ) :
		Kiwi::Entity( canvasName, scene )
	{

		m_canvasRenderTarget = 0;
		m_canvasDimensions = dimensions;
		m_camera = camera;
		m_canvasWidgetScale.Set( 1.0, 1.0 );
		m_redrawWidgets = false;

		m_canvasBgColor = Kiwi::Color( 1.0, 0.0, 0.1, 0.1 ); //default to magenta background color
		m_entityType = ENTITY_2D;
	}

	UICanvas::~UICanvas()
	{
		for( auto widgetItr = m_widgets.begin(); widgetItr != m_widgets.end(); )
		{
			(*widgetItr)->Free();
			widgetItr = m_widgets.erase( widgetItr );
		}
	}

	bool UICanvas::_CreateMesh()
	{
		Kiwi::Mesh* mesh = this->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
		if( mesh != nullptr && mesh->GetModel() != nullptr ) return true;

		if( mesh == nullptr )
		{
			mesh = this->AttachComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
			if( mesh == nullptr ) return false;
		}

		mesh->SetModel( L"Primitive_Quad" );

		/*set the model's material's texture to be the render target*/
		if( m_canvasRenderTarget != nullptr && mesh->GetModel()->GetSubsetCount() > 0 &&
			mesh->GetModel()->GetSubset( 0 )->GetMaterial() != nullptr )
		{
			mesh->GetModel()->GetSubset( 0 )->GetMaterial()->SetTexture( Material::TEXTURE_TYPE::DIFFUSE, m_canvasRenderTarget );
			mesh->GetModel()->GetSubset( 0 )->GetMaterial()->SetShader( L"Basic2DShader" );
		}

		return true;
	}

	void UICanvas::_NotifyWidgetResized( Kiwi::IUIWidget& widget )
	{
		Kiwi::Transform* wTransform = widget.FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		if( wTransform != nullptr )
		{
			wTransform->SetScale( Kiwi::Vector2d( widget.GetWidgetDimensions().x * m_canvasWidgetScale.x, widget.GetWidgetDimensions().y * m_canvasWidgetScale.y ) );
			m_redrawWidgets = true;
		}
	}

	void UICanvas::_NotifyWidgetMoved( Kiwi::IUIWidget& widget )
	{
		/*when a widget's position within the canvas has been updated, this function updates it's transform to the right position and ensures
		that the widget's depth is on top of the canvas*/
		Kiwi::Transform* wTransform = widget.FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		Kiwi::Transform* transform = this->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		if( wTransform != nullptr && transform != nullptr )
		{
			
			if( widget.GetParent() != nullptr )
			{/*for widgets that have a parent, just update the local position*/
				wTransform->SetPosition( widget.GetWidgetPosition() );

			} else
			{/*for widgets without parents, set the local position and add 1 to ensure that all widgets are rendered on top of the canvas*/
				wTransform->SetPosition( widget.GetWidgetPosition() + Kiwi::Vector3d( 0.0, 0.0, transform->GetGlobalPosition().z + 100.0 ) );//  (Kiwi::Vector2d( widget.GetWidgetPosition().x * m_canvasWidgetScale.x, widget.GetWidgetPosition().y * m_canvasWidgetScale.y ));
			}
			m_redrawWidgets = true;
		}
	}

	void UICanvas::_OnStart()
	{
		m_scene->ConnectSceneEventReceiver( *this, L"PreRender", &UICanvas::_OnPreRender );

		Kiwi::Transform* transform = this->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		transform->ConnectTransformEventReceiver( *this, L"Translate", &UICanvas::_OnTranslate );

		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();

		/*create the canvas's render target*/
		if( renderer != nullptr )
		{
			/*get the dimensions of the back buffer so that the render target resolution matches*/
			Kiwi::Vector2d bbDimensions = m_scene->GetRenderer()->GetBackBuffer()->GetDimensions();
			m_canvasWidgetScale.Set( bbDimensions.x / m_canvasDimensions.x, bbDimensions.y / m_canvasDimensions.y );
			m_canvasRenderTarget = renderer->CreateRenderTarget( m_objectName + L"/RenderTarget", *m_scene, bbDimensions );
			if( m_canvasRenderTarget != nullptr )
			{
				m_canvasRenderTarget->SetClearColor( m_canvasBgColor );
				m_canvasRenderTarget->CreateViewport( m_objectName + L"/RenderTarget/Viewport", m_camera );
				m_canvasRenderTarget->CreateRenderGroup( m_objectName, 0 );
				//m_canvasRenderTarget->GetViewport( 0 )->AddRenderGroup( m_objectName );
				//m_canvasRenderTarget->SetAutoRender( false );

				//Kiwi::KiwiBlendDesc blendDesc;
				//blendDesc.desc.BlendEnable = true;
				//blendDesc.desc.SrcBlend = D3D11_BLEND_SRC_ALPHA; //blending for the new top pixel = source color * source alpha
				//blendDesc.desc.DestBlend = D3D11_BLEND_BLEND_FACTOR; //blending for existing pixel, set to use the blend factor to determine blending for each channel
				//blendDesc.desc.BlendOp = D3D11_BLEND_OP_ADD;
				//// dest.rgb = src.rgb * src.a + dest.rgb * (1 - src.a) ==> standard alpha blended color

				//blendDesc.desc.SrcBlendAlpha = D3D11_BLEND_ONE; //use full source alpha
				//blendDesc.desc.DestBlendAlpha = D3D11_BLEND_ZERO; //use no destination alpha
				//blendDesc.desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
				//// dest.a = src.a * (1 - dest.a) + dest.a
				////       == src.a + dest.a - src.a * dest.a
				////       == 1 - (1 - src.a) * (1 - dest.a) ==> preserves alpha to target (optional)
				//blendDesc.desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;// D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_BLUE | D3D11_COLOR_WRITE_ENABLE_GREEN;

				//blendDesc.alphaToCoverageEnable = false;
				//blendDesc.independentBlendEnable = false;

				//Kiwi::BlendState* blendState = renderer->CreateBlendState( m_objectName + L"/RenderTarget/Viewport/TransparencyBlendState", blendDesc );
				//if( blendState != nullptr )
				//{
				//	m_canvasRenderTarget->SetBlendState( blendState );
				//	m_canvasRenderTarget->SetBlendingFactor( Kiwi::Vector4d( 0.0f, 0.0f, 0.0f, 0.0f ) );
				//}
			}
		}

		transform->SetScale( m_canvasDimensions );

		this->_CreateMesh();

		/*initialize the scale and position of all attached widgets*/
		for( auto* widget : m_widgets )
		{
			this->_NotifyWidgetResized( *widget );
			this->_NotifyWidgetMoved( *widget );
		}

		m_redrawWidgets = true;
	}

	void UICanvas::_OnShutdown()
	{
		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( m_canvasRenderTarget != nullptr && renderer != nullptr )
		{
			m_scene->GetRenderer()->DestroyRenderTarget( m_canvasRenderTarget->GetName() );
			m_canvasRenderTarget = 0;
		}
	}

	Kiwi::EventResponse UICanvas::_OnTranslate( std::shared_ptr<Kiwi::TransformEvent> evt )
	{
		m_redrawWidgets = true;

		return Kiwi::EventResponse::NONE;
	}

	void UICanvas::_OnUpdate()
	{
		m_redrawWidgets = true;
		
		for( auto widgetItr = m_widgets.begin(); widgetItr != m_widgets.end(); )
		{
			if( (*widgetItr)->IsShutdown() )
			{
				/*free and remove any widgets that have been shutdown*/
				(*widgetItr)->Free();
				widgetItr = m_widgets.erase( widgetItr );
				continue;

			}

			widgetItr++;
		}
	}

	Kiwi::EventResponse UICanvas::_OnPreRender( std::shared_ptr<Kiwi::SceneEvent> evt )
	{
		if( m_started == false || m_redrawWidgets == false || m_canvasRenderTarget == nullptr ) return Kiwi::EventResponse::NONE;

		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return Kiwi::EventResponse::NONE;
		
		std::vector<Kiwi::Mesh*> meshList;

		m_redrawWidgets = false;
		for( auto widgetItr = m_widgets.begin(); widgetItr != m_widgets.end(); widgetItr++ )
		{
			if( (*widgetItr)->IsShutdown() == false )
			{
				Kiwi::Mesh* widgetMesh = (*widgetItr)->FindComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
				if( widgetMesh != nullptr && widgetMesh->GetModel() != nullptr && widgetMesh->GetModel()->GetSubsetCount() > 0 ) meshList.push_back( widgetMesh );
			}
		}

		//KIWIDEBUGSTRING( Kiwi::ToWString( L"Rendering: " + Kiwi::ToWString( meshList.size() ) ).c_str() );
		renderer->ClearRenderTarget( m_canvasRenderTarget );
		renderer->RenderToTexture( m_canvasRenderTarget, meshList );
		//KIWIDEBUGSTRING( Kiwi::ToWString( L"Done Rendering" ).c_str() );

		return Kiwi::EventResponse::NONE;
	}

	Kiwi::UISprite* UICanvas::CreateSprite( const std::wstring& spriteName, const std::wstring& spriteTexture )
	{
		return nullptr;
		/*KIWIDEBUGSTRING( L"CreateSprite\n" );
		Kiwi::UISprite* sprite = dynamic_cast<Kiwi::UISprite*>(m_scene->CreateEntity( spriteName ));
		if( sprite == nullptr ) return nullptr;*/

		//Kiwi::Mesh* spriteMesh = sprite->AttachComponent<Kiwi::Mesh>( Kiwi::ComponentType::Mesh );
		//if( spriteMesh == nullptr ) return nullptr;

		//Kiwi::StaticModel* model = _kAssetManager.CreatePrimitiveModel( spriteName + L"/Mesh/Model", L"quad" );
		//if( model == nullptr ) return;

		//meshComponent->SetModel( model );

		///*set the model's material's texture to be the render target*/
		//if( m_canvasRenderTarget != nullptr && meshComponent->GetModel()->GetSubsetCount() > 0 &&
		//	meshComponent->GetModel()->GetSubset( 0 )->GetMaterial() != nullptr )
		//{
		//	meshComponent->GetModel()->GetSubset( 0 )->GetMaterial()->SetTexture( Material::TEXTURE_TYPE::DIFFUSE, m_canvasRenderTarget );
		//}

		////if a widget shader has been set, tell the new sprite to use it
		//if( m_widgetShader.compare( L"" ) != 0 )
		//{
		//	Kiwi::IModel* spriteModel = spriteMesh->GetModel();
		//	if( spriteModel == nullptr ) return nullptr;

		//	if( spriteModel->GetSubsetCount() > 0 && spriteModel->GetSubsetCount() > 0 )
		//	{
		//		spriteModel->GetSubset( 0 )->GetMaterial()->SetShader( m_widgetShader );
		//	}
		//}

		//spriteMesh->SetRenderGroup( m_objectName );

		//sprite->Reserve();
		//m_widgets.push_back( sprite );
		//m_redrawWidgets = true;

		//return sprite;
	}

	Kiwi::UIButton* UICanvas::CreateButton( const std::wstring& buttonName, const std::wstring& buttonTexture )
	{
		return nullptr;
		/*KIWIDEBUGSTRING( L"CreateButton\n" );
		Kiwi::UIButton* button = nullptr;
		
		button = dynamic_cast<Kiwi::UIButton*>(m_scene->CreateEntity( buttonName, Kiwi::MakeType::StringPair( { { L"EntityType", L"UIWidget" },
																												{ L"WidgetType", L"UIButton" },
																												{ L"UICanvas", m_objectName },
																												{ L"Texture0", buttonTexture } } ) ));

		if( button != nullptr )
		{
			button->Reserve();
			m_widgets.push_back( button );
			m_redrawWidgets = true;
		}
		
		return button;*/
	}

	Kiwi::UITextbox* UICanvas::CreateTextbox( const std::wstring& textboxName, const std::wstring& font, const std::wstring& textboxTexture, const std::wstring& initialText )
	{
		Kiwi::UITextbox* textbox = dynamic_cast<Kiwi::UITextbox*>(m_scene->AddGameObject( std::make_unique<Kiwi::UITextbox>( m_objectName + L"/" + textboxName, *this, font, textboxTexture, initialText ) ));
		if( textbox == nullptr ) return nullptr;

		textbox->Reserve();
		m_widgets.push_back( textbox );
		m_redrawWidgets = true;

		return textbox;
	}

	Kiwi::UIScrollBar* UICanvas::CreateScrollBar( const std::wstring& scrollbarName, const std::wstring& buttonTexture, const std::wstring& sliderTexture, const std::wstring& backgroundTexture )
	{
		return nullptr;
		/*KIWIDEBUGSTRING( L"CreateScrollBar\n" );
		Kiwi::UIScrollBar* scrollbar = dynamic_cast<Kiwi::UIScrollBar*>(m_scene->CreateEntity( scrollbarName, Kiwi::MakeType::StringPair( { { L"EntityType", L"UIWidget" },
																																			{ L"WidgetType", L"UIScrollBar" },
																																			{ L"UICanvas", m_objectName },
																																			{ L"Texture0", buttonTexture },
																																			{ L"Texture1", sliderTexture },
																																			{ L"Texture2", backgroundTexture } } ) ));
		if( scrollbar != nullptr )
		{
			scrollbar->Reserve();
			m_widgets.push_back( scrollbar );
			m_redrawWidgets = true;
		}

		return scrollbar;*/
	}

	void UICanvas::SetCamera( Kiwi::Camera* camera )
	{
		m_camera = camera;

		if( m_canvasRenderTarget != 0 && m_canvasRenderTarget->GetViewportCount() != 0 )
		{
			m_canvasRenderTarget->GetViewport( 0 )->AttachCamera( m_camera );
		}
	}

	void UICanvas::SetBackgroundColor( const Kiwi::Color& bgColor )
	{
		m_canvasBgColor = bgColor;

		if( m_canvasRenderTarget != 0 )
		{
			m_canvasRenderTarget->SetClearColor( m_canvasBgColor );
		}
	}

	Kiwi::Vector2d UICanvas::GetWidgetScreenPosition( Kiwi::IUIWidget& widget )
	{
		Kiwi::Transform* widgetTransform = widget.FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		Kiwi::Transform* transform = this->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		if( widget.GetWidgetCanvas() == this )
		{
			return Kiwi::Vector2d( widgetTransform->GetGlobalPosition().x + transform->GetGlobalPosition().x,
								   widgetTransform->GetGlobalPosition().y + transform->GetGlobalPosition().y );

		} else
		{
			return Kiwi::Vector2d();
		}
	}

}