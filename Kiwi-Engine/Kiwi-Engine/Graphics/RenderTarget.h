#ifndef _KIWI_RENDERTARGET_H_
#define _KIWI_RENDERTARGET_H_

#include "Viewport.h"
#include "DirectX.h"
#include "Color.h"
#include "Texture.h"
#include "BlendState.h"
#include "RenderQueue.h"

#include "../Core/Vector2d.h"
#include "..\Core\Vector4d.h"

#include <vector>

namespace Kiwi
{

	class Camera;
	class Renderer;
	class DepthStencil;
	class Scene;
	class RenderQueue;

	class RenderTarget:
		public Kiwi::Texture
	{
	protected:

		Kiwi::Scene* m_renderTargetScene;

		std::wstring m_name;

		std::vector<Kiwi::Viewport> m_viewports;

		ID3D11RenderTargetView* m_renderTargetView;

		Kiwi::DepthStencil* m_depthStencil;

		Kiwi::Vector2d m_dimensions;

		Kiwi::Vector4d m_blendingFactor; //amount of each color channel to use in blending

		KiwiBlendDesc m_blendDesc;

		//the active blend state used for rendering
		//this state will be set in the renderer at the time the render target is bound
		Kiwi::BlendState* m_blendState;

		std::wstring m_blendStateName;

		//stores the color the render target will be cleared with
		Kiwi::Color m_clearColor;

		unsigned int m_priorityLevel;

		Kiwi::RenderQueue m_renderQueue;

		/*the render target will only be rendered and rendered to if this is true*/
		bool m_isActive;

		/*if true the renderer will automatically render this render target's render groups each frame
		if false the render target has to be manually drawn to*/
		bool m_autoRenderEnabled;

	public:

		RenderTarget( std::wstring name, Kiwi::Scene* scene, Kiwi::Renderer& renderer, const Kiwi::Vector2d& dimensions );

		RenderTarget( std::wstring name, Kiwi::Scene* scene, Kiwi::Renderer& renderer, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource, ID3D11RenderTargetView* renderTargetView );

		virtual ~RenderTarget();

		virtual void AttachDepthStencil(Kiwi::DepthStencil* depthStencil);

		bool AutoRenderEnabled()const { return m_autoRenderEnabled; }

		/*creates and adds a viewport to this render target
		the position and dimension parameters are relative to the size of the render target (between 0-1)*/
		virtual Kiwi::Viewport* CreateViewport( std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2d& position = Kiwi::Vector2d( 0.0, 0.0 ), const Kiwi::Vector2d& dimensions = Kiwi::Vector2d( 1.0, 1.0 ), float minDepth = 0.0f, float maxDepth = 1.0f );

		void CreateRenderGroup( const std::wstring& groupName, unsigned short viewport );

		const Kiwi::KiwiBlendDesc& GetBlendDesc()const { return m_blendDesc; }

		const Kiwi::Vector4d& GetBlendingFactor()const { return m_blendingFactor; }

		//returns the current active blend state
		Kiwi::BlendState* GetBlendState()const { return m_blendState; }

		const std::wstring& GetBlendStateName()const { return m_blendStateName; }

		const Kiwi::Color& GetClearColor()const { return m_clearColor; }

		Kiwi::DepthStencil* GetDepthStencil()const { return m_depthStencil; }

		const Kiwi::Vector2d& GetDimensions()const { return m_dimensions; }

		//Kiwi::Scene* GetScene()const { return m_scene; }
		std::wstring GetName()const { return m_name; }

		//returns the render priority of the render target. targets with higher priority are drawn after those with lower priorities
		unsigned int GetPriority()const { return m_priorityLevel; }

		Kiwi::RenderQueue& GetRenderQueue() { return m_renderQueue; }

		ID3D11RenderTargetView* GetRenderTargetView()const;

		Kiwi::Viewport* GetViewport( unsigned int index );

		unsigned int GetViewportCount() { return (unsigned int)m_viewports.size(); }

		bool IsActive()const { return m_isActive; }

		void SetActive( bool isActive ) { m_isActive = isActive; }

		void SetAutoRender( bool autoRenderEnabled ) { m_autoRenderEnabled = autoRenderEnabled; }

		void SetBlendingFactor( const Kiwi::Vector4d& blendFactor );

		void SetBlendState( const std::wstring& name );

		void SetBlendState( Kiwi::BlendState* blendState );

		virtual void SetClearColor( const Kiwi::Color& clearColor );

		/*sets the priority level. render targets with higher priorities are rendered
		before those with low priorities. all render targets default to a priority of 0*/
		void SetPriority( unsigned int priorityLevel ) { m_priorityLevel = priorityLevel; }

		void OnPreRender();

		void OnPostRender();

	};
};

#endif