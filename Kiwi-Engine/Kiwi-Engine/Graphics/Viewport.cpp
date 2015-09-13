#include "Viewport.h"
#include "Camera.h"

#include "../Core/Exception.h"

namespace Kiwi
{

	Viewport::Viewport( std::wstring name, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, float minDepth, float maxDepth )
	{

		m_camera = 0;
		m_name = name;

		m_position.Set( position );
		m_dimensions.Set( dimensions );
		m_minDepth = minDepth;
		m_maxDepth = maxDepth;

		m_viewport.Width = m_dimensions.x;
		m_viewport.Height = m_dimensions.y;
		m_viewport.MaxDepth = m_maxDepth;
		m_viewport.MinDepth = m_minDepth;
		m_viewport.TopLeftX = m_position.x;
		m_viewport.TopLeftY = m_position.y;

	}

	Viewport::Viewport( std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, float minDepth, float maxDepth )
	{

		m_camera = camera;
		m_name = name;

		m_position.Set( position );
		m_dimensions.Set( dimensions );
		m_minDepth = minDepth;
		m_maxDepth = maxDepth;

		m_viewport.Width = m_dimensions.x;
		m_viewport.Height = m_dimensions.y;
		m_viewport.MaxDepth = m_maxDepth;
		m_viewport.MinDepth = m_minDepth;
		m_viewport.TopLeftX = m_position.x;
		m_viewport.TopLeftY = m_position.y;

		if(m_camera) m_camera->SetViewport( this );

	}

	void Viewport::AttachCamera(Kiwi::Camera* camera)
	{

		m_camera = camera;
		if( m_camera ) camera->SetViewport( this );

	}

};