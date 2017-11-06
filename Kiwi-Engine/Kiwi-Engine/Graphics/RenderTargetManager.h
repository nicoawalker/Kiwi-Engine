#ifndef _KIWI_RENDERTARGETMANAGER_H_
#define _KIWI_RENDERTARGETMANAGER_H_

#include "RenderTarget.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace Kiwi
{

	class Renderer;

	struct RenderTargetList
	{
		std::vector<Kiwi::RenderTarget*> targets;
	};

	class RenderTargetManager
	{
	protected:

		Kiwi::Renderer* m_renderer;

		std::unordered_map<std::wstring, std::unique_ptr<Kiwi::RenderTarget>> m_renderTargets;

	public:

		RenderTargetManager( Kiwi::Renderer& renderer );
		~RenderTargetManager() {}

		Kiwi::RenderTarget* CreateRenderTarget( std::wstring name, Kiwi::Scene* scene, const Kiwi::Vector2d& dimensions );

		Kiwi::RenderTarget* CreateRenderTarget( std::wstring name, Kiwi::Scene* scene, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource, ID3D11RenderTargetView* renderTargetView );

		/*returns the render target with matching name, or 0 if no match is found*/
		Kiwi::RenderTarget* FindRenderTarget( std::wstring name );

		/*destroys the render target with matching name*/
		void DestroyRenderTarget( std::wstring name );

		Kiwi::RenderTargetList GetRenderTargetList();

		void Shutdown();

	};

}

#endif