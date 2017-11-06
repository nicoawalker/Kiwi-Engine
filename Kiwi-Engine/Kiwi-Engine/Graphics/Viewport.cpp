#include "Viewport.h"
#include "Camera.h"
#include "RenderTarget.h"

#include "../Core/Exception.h"

namespace Kiwi
{

	Viewport::Viewport( Kiwi::RenderTarget& renderTarget, std::wstring name, const Kiwi::Vector2d& position, const Kiwi::Vector2d& dimensions, float minDepth, float maxDepth )
	{
		m_camera = 0;
		m_renderTarget = &renderTarget;
		m_name = name;

		m_position.Set( position );
		m_dimensions.Set( dimensions );
		m_minDepth = minDepth;
		m_maxDepth = maxDepth;

		m_viewport.Width = (float)m_dimensions.x;
		m_viewport.Height = (float)m_dimensions.y;
		m_viewport.MaxDepth = (float)m_maxDepth;
		m_viewport.MinDepth = (float)m_minDepth;
		m_viewport.TopLeftX = (float)m_position.x;
		m_viewport.TopLeftY = (float)m_position.y;
	}

	Viewport::Viewport( Kiwi::RenderTarget& renderTarget, std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2d& position, const Kiwi::Vector2d& dimensions, float minDepth, float maxDepth )
	{
		m_renderTarget = &renderTarget;
		m_camera = camera;
		m_name = name;

		m_position.Set( position );
		m_dimensions.Set( dimensions );
		m_minDepth = minDepth;
		m_maxDepth = maxDepth;

		m_viewport.Width = (float)m_dimensions.x;
		m_viewport.Height = (float)m_dimensions.y;
		m_viewport.MaxDepth = (float)m_maxDepth;
		m_viewport.MinDepth = (float)m_minDepth;
		m_viewport.TopLeftX = (float)m_position.x;
		m_viewport.TopLeftY = (float)m_position.y;
	}

	void Viewport::AttachCamera(Kiwi::Camera* camera)
	{
		m_camera = camera;
	}

	void Viewport::AddRenderGroup( std::wstring renderGroup )
	{
		m_renderGroups.push_back( renderGroup );
	}

	void Viewport::SetDimensions( const Kiwi::Vector2d& dimensions )
	{
		if( m_renderTarget == 0 )
		{
			throw Kiwi::Exception( L"Viewport::SetDimensions", L"Failed to change viewport '" + m_name + L"': no RenderTarget", KIWI_EXCEPTION::NULLOPERATION );
		}

		m_dimensions.Set( dimensions.x * m_renderTarget->GetDimensions().x, dimensions.y * m_renderTarget->GetDimensions().y );

		m_viewport.Width = (float)m_dimensions.x;
		m_viewport.Height = (float)m_dimensions.y;
	}

	void Viewport::SetPosition( const Kiwi::Vector2d& position )
	{
		if( m_renderTarget == 0 )
		{
			throw Kiwi::Exception( L"Viewport::SetPosition", L"Failed to change viewport '" + m_name + L"': no RenderTarget", KIWI_EXCEPTION::NULLOPERATION );
		}

		m_position.Set( position.x * m_renderTarget->GetDimensions().x, position.y * m_renderTarget->GetDimensions().y );

		m_viewport.TopLeftX = (float)m_position.x;
		m_viewport.TopLeftY = (float)m_position.y;
	}

};