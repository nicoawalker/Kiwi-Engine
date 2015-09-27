#ifndef _KIWI_RENDERER_H_
#define _KIWI_RENDERER_H_

#include "DirectX.h"
#include "RasterStateManager.h"
#include "Color.h"

#include "../Core/Vector2.h"
#include "../Core/Vector4.h"

#include <string>

namespace Kiwi
{

	struct BackBuffer;

	class D3D11Interface;
	class RenderWindow;
	class RenderTarget;
	class DepthStencil;
	class BlendState;

	enum PrimitiveTopology
	{
		POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	struct MultisampleDesc
	{
		//The number of multisamples per pixel.
		unsigned int count;

		/*The image quality level. The higher the quality, the lower the performance.
		The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultisampleQualityLevels*/
		unsigned int quality;
	};

	class Renderer
	{
	private:

		std::wstring m_name;

		Kiwi::D3D11Interface* m_d3dInterface;

		Kiwi::RenderWindow* m_renderWindow;

		//the render target that is currently set to be drawn to
		Kiwi::RenderTarget* m_activeRenderTarget;

		Kiwi::RenderTarget* m_backBuffer;

		Kiwi::RasterStateManager* m_rasterStateManager;

	public:

		Renderer(std::wstring name, Kiwi::RenderWindow* window);
		~Renderer();

		void Present();

		/*creates a new render target with the given name and dimensions. 
		If a pre-initialized texture is passed (e.g. a back buffer) it is used and a new texture is not created*/
		//Kiwi::RenderTarget* CreateRenderTarget(std::wstring name, const Kiwi::Vector2& dimensions, ID3D11Texture2D* texture = 0);

		/*clears the currently active render target, including its depth buffer*/
		void ClearRenderTarget();
		/*clears the currently active render target, including its depth buffer with the given color*/
		void ClearRenderTarget(const Kiwi::Color& color);
		/*clears the passed render target, including its depth buffer*/
		void ClearRenderTarget( Kiwi::RenderTarget* rt );

		Kiwi::RasterState* CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc );

		void SetPrimitiveTopology(Kiwi::PrimitiveTopology topology);

		void SetRenderTarget(Kiwi::RenderTarget* target);
		/*sets the back buffer to be the active render target*/
		void SetBackBufferRenderTarget();

		void SetRasterState( Kiwi::RasterState* rasterState );
		void SetRasterState( std::wstring name );

		void SetBlendState( Kiwi::BlendState* blendState );

		void DestroyAllRasterStates();
		void DestroyRasterStateWithName( std::wstring name );

		std::wstring GetName()const { return m_name; }

		DXGI_FORMAT GetDXGIFormat()const;
		ID3D11Device* GetDevice()const;
		ID3D11DeviceContext* GetDeviceContext()const;

		Kiwi::RenderTarget* GetActiveRenderTarget()const { return m_activeRenderTarget; }
		Kiwi::RasterState* FindRasterStateWithName( std::wstring name );
		Kiwi::MultisampleDesc GetMultisampleDesc();
		Kiwi::RenderTarget* GetBackBuffer()const { return m_backBuffer; }

	};
};

#endif