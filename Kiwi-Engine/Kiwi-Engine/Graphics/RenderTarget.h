#ifndef _KIWI_RENDERTARGET_H_
#define _KIWI_RENDERTARGET_H_

#include "BlendStateManager.h"
#include "Viewport.h"
#include "DirectX.h"
#include "Color.h"

#include "../Core/Vector2.h"

#include <vector>

namespace Kiwi
{

	class Camera;
	class Renderer;
	class DepthStencil;

	class RenderTarget
	{
	protected:

		std::wstring m_name;

		std::vector<Kiwi::Viewport> m_viewports;

		ID3D11Texture2D* m_renderTexture;
		ID3D11RenderTargetView* m_renderView;
		ID3D11ShaderResourceView* m_shaderResource;

		Kiwi::DepthStencil* m_depthStencil;

		Kiwi::Vector2 m_dimensions;

		D3DRenderTargetBlendDesc m_blendDesc;

		Kiwi::BlendStateManager* m_blendStateManager;

		//the active blend state used for rendering
		//this state will be set in the renderer at the time the render target is bound
		Kiwi::BlendState* m_activeBlendState;

		//stores the color the render target will be cleared with
		Kiwi::Color m_clearColor;

		unsigned int m_priorityLevel;

	public:

		RenderTarget( std::wstring name, Kiwi::Renderer* renderer, unsigned int width, unsigned int height );
		RenderTarget( std::wstring name, Kiwi::Renderer* renderer, ID3D11Texture2D* texture, ID3D11RenderTargetView* renderTargetView );
		//RenderTarget(std::wstring name, Kiwi::Scene* scene, Kiwi::Texture* renderTexture);
		virtual ~RenderTarget();

		void RenderFrame() {}
		void RenderAll() {}

		virtual void AttachDepthStencil(Kiwi::DepthStencil* depthStencil);

		/*sets the priority level. render targets with higher priorities are rendered
		before those with low priorities. all render targets default to a priority of 0*/
		void SetPriority( unsigned int priorityLevel ) { m_priorityLevel = priorityLevel; }

		/*sets the active blend state for the render target*/
		void SetBlendState( std::wstring blendStateName );

		void SetClearColor( const Kiwi::Color& clearColor ) { m_clearColor = clearColor; }

		/*creates and adds a viewport to this render target
		the position and dimension parameters are relative to the size of the render target (between 0-1)*/
		virtual Kiwi::Viewport* CreateViewport( std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2& position = Kiwi::Vector2(0.0f, 0.0f), const Kiwi::Vector2& dimensions = Kiwi::Vector2(1.0f, 1.0f), float minDepth = 0.0f, float maxDepth = 1.0f );

		virtual Kiwi::BlendState* CreateBlendState( std::wstring name, const D3DBlendDesc& blendDesc, const Kiwi::Vector4& blendFactor, unsigned int sampleMask = 0xFFFFFF );

		virtual Kiwi::BlendState* FindBlendStateWithName( std::wstring name );

		//Kiwi::Scene* GetScene()const { return m_scene; }
		std::wstring GetName()const { return m_name; }
		const Kiwi::Vector2& GetDimensions()const { return m_dimensions; }
		const Kiwi::Color& GetClearColor()const { return m_clearColor; }
		Kiwi::Viewport* GetViewport(unsigned int index);

		unsigned int GetViewportCount() { return (unsigned int)m_viewports.size(); }
		unsigned int GetPriority()const { return m_priorityLevel; }

		//returns the current active blend state
		Kiwi::BlendState* GetBlendState()const { return m_activeBlendState; }
		const Kiwi::D3DRenderTargetBlendDesc& GetBlendDesc()const { return m_blendDesc; }

		ID3D11Texture2D* GetTexture()const { return m_renderTexture; }
		ID3D11RenderTargetView* GetView()const { return m_renderView; }
		ID3D11ShaderResourceView* GetShaderResourceView()const { return m_shaderResource; }
		Kiwi::DepthStencil* GetDepthStencil()const { return m_depthStencil; }

	};
};

#endif