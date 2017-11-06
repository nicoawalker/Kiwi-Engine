#include "Camera.h"
#include "Viewport.h"
#include "RenderWindow.h"

#include "../Core/Math.h"
#include "../Core/Transform.h"
#include "../Core/Exception.h"
#include "../Core/Scene.h"

namespace Kiwi
{

	Camera::Camera(std::wstring name, Kiwi::Scene& scene) : 
		Kiwi::Entity(name, scene)
	{
		assert( m_scene->GetRenderWindow() != 0);

		m_fov = 0.785398f; //PI / 4
		m_aspectRatio = 1.777777f; //16:9
		m_farClipDistance = 200.0f;
		m_nearClipDistance = 1.0f;

		m_projMatrix = Kiwi::Matrix4::Projection( m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance );

		Kiwi::Vector2 windowSize = m_scene->GetRenderWindow()->GetClientSize();

		m_orthoMatrix = Kiwi::Matrix4::Orthographic( windowSize.x, windowSize.y, m_nearClipDistance, m_farClipDistance );

		//setup 2d view matrix
		m_viewMatrix2D.a1 = 1.0f;
		m_viewMatrix2D.b2 = 1.0f;
		m_viewMatrix2D.c3 = 1.0f;
		m_viewMatrix2D.d3 = 1.0f;
		m_viewMatrix2D.d4 = 1.0f;

		//lock the camera from rolling

		this->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->LockAxisTo( Kiwi::AXIS::Z_AXIS, 0.0f );

		this->_CalculateViewMatrix();
	}

	Camera::Camera( std::wstring name, Kiwi::Scene& scene, float FOV, float aspectRatio, float nearClip, float farClip ):
		Kiwi::Entity( name, scene )
	{
		assert( m_scene != 0 && m_scene->GetRenderWindow() != 0 );

		m_fov = FOV;
		m_aspectRatio = aspectRatio;
		m_farClipDistance = farClip;
		m_nearClipDistance = nearClip;

		m_projMatrix = Kiwi::Matrix4::Projection( m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance );

		Kiwi::Vector2 windowSize = m_scene->GetRenderWindow()->GetClientSize();

		m_orthoMatrix = Kiwi::Matrix4::Orthographic( windowSize.x, windowSize.y, m_nearClipDistance, m_farClipDistance );

		//setup 2d view matrix
		m_viewMatrix2D.a1 = 1.0f;
		m_viewMatrix2D.b2 = 1.0f;
		m_viewMatrix2D.c3 = 1.0f;
		m_viewMatrix2D.d3 = 1.0f;
		m_viewMatrix2D.d4 = 1.0f;

		//lock the camera from rolling
		this->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->LockAxisTo( Kiwi::AXIS::Z_AXIS, 0.0f );

		this->_CalculateViewMatrix();
	}

	void Camera::_CalculateViewMatrix()
	{
		Kiwi::Transform* transform = this->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );

		//get the local x-y-z axis for the camera
		Kiwi::Vector3d front = transform->GetForward();
		Kiwi::Vector3d up = transform->GetUp();
		Kiwi::Vector3d right = transform->GetRight();

		Kiwi::Vector3d pos = transform->GetGlobalPosition();

		//recalculate the view matrix
		m_viewMatrix.d1 = -right.Dot( pos );
		m_viewMatrix.d2 = -up.Dot( pos );
		m_viewMatrix.d3 = -front.Dot( pos );

		m_viewMatrix.a1 = right.x;
		m_viewMatrix.a2 = up.x;
		m_viewMatrix.a3 = front.x;

		m_viewMatrix.b1 = right.y;
		m_viewMatrix.b2 = up.y;
		m_viewMatrix.b3 = front.y;

		m_viewMatrix.c1 = right.z;
		m_viewMatrix.c2 = up.z;
		m_viewMatrix.c3 = front.z;

		m_viewMatrix.d4 = 1.0f;
	}

	void Camera::SetViewport( Kiwi::Viewport* viewport )
	{
		//create projection matrix to convert 3d space into 2d coordinates
		m_aspectRatio = (float)(viewport->GetDimensions().x / viewport->GetDimensions().y);
		m_projMatrix = Kiwi::Matrix4::Projection( m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance );
		//create 2d orthographic matrix
		m_orthoMatrix = Kiwi::Matrix4::Orthographic( viewport->GetDimensions().x, viewport->GetDimensions().y, m_nearClipDistance, m_farClipDistance );
	}

	void Camera::Update()
	{
		this->_CalculateViewMatrix();
	}

	void Camera::SetNearClipDistance(float distance)
	{
		m_nearClipDistance = distance;
	}

	void Camera::SetFarClipDistance(float distance)
	{
		m_farClipDistance = distance;
	}

	void Camera::SetFOV(float fov)
	{
		m_fov = fov;

		m_projMatrix = Kiwi::Matrix4::Projection(m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance);
	}

	void Camera::SetAspectRatio( double aspectRatio )
	{
		m_aspectRatio = (float)aspectRatio;

		m_projMatrix = Kiwi::Matrix4::Projection( m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance );
	}

};