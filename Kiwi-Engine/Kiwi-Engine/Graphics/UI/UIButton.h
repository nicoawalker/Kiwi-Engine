#ifndef _KIWI_UIBUTTON_H_
#define _KIWI_UIBUTTON_H_

#include "IUIWidget.h"

#include "..\..\Events\EventHandling.h"
#include "../../Events/IUIButtonEventEmitter.h"

#include "../../Core/Component.h"
#include "..\..\Core\Vector2d.h"

#include <string>

namespace Kiwi
{

	class UISprite;
	class UITextbox;
	class UICanvas;

	class UIButton :
		public Kiwi::IUIWidget,
		public Kiwi::IInputEventReceiver,
		public Kiwi::IUIButtonEventEmitter
	{
	protected:

		Kiwi::UISprite* m_buttonSprite; //sprite image of the button (can be animated)

		/*region around the button where the button will remain pressed*/
		Kiwi::Vector2d m_holdRegion;

		//Kiwi::UITextBox* m_labelTextBox; //text box above the button with the button's label

		std::wstring m_labelFont; //font used for the button label
		std::wstring m_label; //button label

		std::wstring m_textureName; //name of the button's texture, if present

		bool m_mouseOver; //true if the mouse is currently over the button
		bool m_isPressed; //true if the button is currently being held down

	protected:

		virtual void _OnStart();
		virtual void _OnShutdown();

		void _OnUpdate();
		void _OnWidgetResized( const Kiwi::Vector2d& resizeAmount );

		Kiwi::EventResponse _OnMousePress( std::shared_ptr<Kiwi::InputEvent> evt );
		Kiwi::EventResponse _OnMouseReleased( std::shared_ptr<Kiwi::InputEvent> evt );
		Kiwi::EventResponse _OnMouseMove( std::shared_ptr<Kiwi::InputEvent> evt );

		//called every frame as long as m_isPressed is true
		void _OnMouseHeld();

	public:

		/*creates an unlabeled button*/
		UIButton( std::wstring name, Kiwi::UICanvas& canvas, std::wstring buttonTextureName = L"" );

		/*creates a button with a label*/
		UIButton( std::wstring name, Kiwi::UICanvas& canvas, std::wstring label, std::wstring labelFont, std::wstring buttonTextureName = L"" );
		virtual ~UIButton();

		/*triggers one click of the button*/
		void Trigger() {}

		void SetTexture( std::wstring textureName );
		//void SetLabel( std::wstring label );

		bool IsPressed()const { return m_isPressed; }

		Kiwi::UISprite* GetSprite()const { return m_buttonSprite; }
		std::wstring GetLabel()const { return m_label; }

	};
}

#endif