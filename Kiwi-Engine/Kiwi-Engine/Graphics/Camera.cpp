#include "Camera.h"
#include "Viewport.h"

#include "../Core/Math.h"
#include "../Core/Transform.h"
#include "../Core/Exception.h"

namespace Kiwi
{

	Camera::Camera(std::wstring name)://, Kiwi::Scene* scene) : 
		Kiwi::Entity(name, 0)
	{

		m_fov = 0.785398f; //PI / 4
		m_aspectRatio = 1.777777f; //16:9
		m_farClipDistance = 200.0f;
		m_nearClipDistance = 1.0f;

		m_projMatrix = Kiwi::Matrix4::Projection( m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance );

		m_orthoMatrix = Kiwi::Matrix4::Orthographic( 800, 600, m_nearClipDistance, m_farClipDistance );

		//setup 2d view matrix
		m_viewMatrix2D.a1 = 1.0f;
		m_viewMatrix2D.b2 = 1.0f;
		m_viewMatrix2D.c3 = 1.0f;
		m_viewMatrix2D.d3 = 1.0f;
		m_viewMatrix2D.d4 = 1.0f;

		//lock the camera from rolling
		m_transform.LockAxisTo(Kiwi::AXIS::Z_AXIS, 0.0f);

		this->_CalculateViewMatrix();

	}

	Camera::Camera( std::wstring name, /*Kiwi::Scene* scene,*/ float FOV, float aspectRatio, float nearClip, float farClip ):
		Kiwi::Entity( name, 0 )
	{

		m_fov = FOV;
		m_aspectRatio = aspectRatio;
		m_farClipDistance = farClip;
		m_nearClipDistance = nearClip;

		m_projMatrix = Kiwi::Matrix4::Projection( m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance );

		m_orthoMatrix = Kiwi::Matrix4::Orthographic( 800, 600, m_nearClipDistance, m_farClipDistance );

		//setup 2d view matrix
		m_viewMatrix2D.a1 = 1.0f;
		m_viewMatrix2D.b2 = 1.0f;
		m_viewMatrix2D.c3 = 1.0f;
		m_viewMatrix2D.d3 = 1.0f;
		m_viewMatrix2D.d4 = 1.0f;

		//lock the camera from rolling
		m_transform.LockAxisTo( Kiwi::AXIS::Z_AXIS, 0.0f );

		this->_CalculateViewMatrix();

	}

	void Camera::_CalculateViewMatrix()
	{

		//get the local x-y-z axis for the camera
		Kiwi::Vector3 front = m_transform.GetForward();
		Kiwi::Vector3 up = m_transform.GetUp();
		Kiwi::Vector3 right = m_transform.GetRight();

		Kiwi::Vector3 pos = m_transform.GetPosition();

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

	void Camera::SetViewport(Kiwi::Viewport* viewport)
	{

		if(viewport == 0)
		{
			throw Kiwi::Exception(L"Camera::Initialize", L"Invalid viewport");
		}

		m_viewport = viewport;

		m_viewportDimensions = viewport->GetDimensions();
		
		//create projection matrix to convert 3d space into 2d coordinates
		m_aspectRatio = m_viewportDimensions.x / m_viewportDimensions.y;
		m_projMatrix = Kiwi::Matrix4::Projection(m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance);
		//create 2d orthographic matrix
		m_orthoMatrix = Kiwi::Matrix4::Orthographic(m_viewportDimensions.x, m_viewportDimensions.y, m_nearClipDistance, m_farClipDistance);

	}

	void Camera::Update()
	{

		this->_CalculateViewMatrix();

	}

	void Camera::SetNearClipDistance(float distance)
	{

		m_nearClipDistance = distance;

		m_projMatrix = Kiwi::Matrix4::Projection(m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance);
		m_orthoMatrix = Kiwi::Matrix4::Orthographic(m_viewportDimensions.x, m_viewportDimensions.y, m_nearClipDistance, m_farClipDistance);

	}

	void Camera::SetFarClipDistance(float distance)
	{

		m_farClipDistance = distance;

		m_projMatrix = Kiwi::Matrix4::Projection(m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance);
		m_orthoMatrix = Kiwi::Matrix4::Orthographic(m_viewportDimensions.x, m_viewportDimensions.y, m_nearClipDistance, m_farClipDistance);

	}

	void Camera::SetFOV(float fov)
	{

		m_fov = fov;

		m_projMatrix = Kiwi::Matrix4::Projection(m_fov, m_aspectRatio, m_nearClipDistance, m_farClipDistance);

	}

};