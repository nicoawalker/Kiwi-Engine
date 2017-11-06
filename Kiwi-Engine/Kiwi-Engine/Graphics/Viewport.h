#ifndef _KIWI_VIEWPORT_H_
#define _KIWI_VIEWPORT_H_

#include "../Core/Vector2d.h"

#include "DirectX.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class Camera;
	class RenderTarget;

	class Viewport
	{
	protected:

		/*pointer to the render target that created the viewport*/
		Kiwi::RenderTarget* m_renderTarget;

		Kiwi::Camera* m_camera;

		Kiwi::Vector2d m_position;
		Kiwi::Vector2d m_dimensions;

		std::vector<std::wstring> m_renderGroups;

		float m_minDepth;
		float m_maxDepth;

		std::wstring m_name;

		D3D11_VIEWPORT m_viewport;

	public:

		Viewport( Kiwi::RenderTarget& renderTarget, std::wstring name, const Kiwi::Vector2d& position, const Kiwi::Vector2d& dimensions, float minDepth, float maxDepth );
		Viewport( Kiwi::RenderTarget& renderTarget, std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2d& position, const Kiwi::Vector2d& dimensions, float minDepth, float maxDepth );
		virtual ~Viewport() {}

		void AttachCamera(Kiwi::Camera* camera);

		void AddRenderGroup( std::wstring renderGroup );

		/*sets the dimensions for the viewport in percent, relative to the size of the render target, between 0 and 1*/
		void SetDimensions( const Kiwi::Vector2d& dimensions );
	
		/*sets the position of the viewport in the render target
		coordinates are in percent relative to the size of the render target, between 0 and 1*/
		void SetPosition( const Kiwi::Vector2d& position );

		Kiwi::Camera* GetCamera()const { return m_camera; }

		std::wstring GetName()const { return m_name; }

		float GetMinDepth()const { return m_minDepth; }
		float GetMaxDepth()const { return m_maxDepth; }

		bool UsingDefaultRenderGroup()const { return m_renderGroups.size() == 0; }

		std::vector<std::wstring>& GetRenderGroupList() { return m_renderGroups; }

		const Kiwi::Vector2d& GetDimensions()const { return m_dimensions; }
		const Kiwi::Vector2d& GetPosition()const { return m_position; }

		D3D11_VIEWPORT& GetD3DViewport() { return m_viewport; }

	};
};

#endif