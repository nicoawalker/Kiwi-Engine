#ifndef _KIWI_CAMERA_H_
#define _KIWI_CAMERA_H_

#include "../Core/Matrix4.h"
#include "../Core/Vector3.h"
#include "../Core/Vector2.h"
#include "../Core/Entity.h"

#include <string>
#include <set>

namespace Kiwi
{
	
	class Viewport;

	class Camera: 
		public Kiwi::Entity
	{
	protected:

		Kiwi::Matrix4 m_viewMatrix;
		Kiwi::Matrix4 m_viewMatrix2D;
		Kiwi::Matrix4 m_projMatrix;
		Kiwi::Matrix4 m_orthoMatrix;

		float m_aspectRatio;
		float m_fov;
		float m_nearClipDistance;
		float m_farClipDistance;

	protected:

		void _CalculateViewMatrix();

	public:

		Camera( std::wstring name, Kiwi::Scene& scene);
		Camera( std::wstring name, Kiwi::Scene& scene, float FOV, float aspectRatio, float nearClip, float farClip );
		virtual ~Camera() {}

		/*Builds the camera's orthographic projection matrix for the viewport*/
		void SetViewport( Kiwi::Viewport* viewport );

		/*creates the view matrix based on the camera's current rotation and position*/
		virtual void Update();

		void SetNearClipDistance(float distance);
		void SetFarClipDistance(float distance);
		void SetFOV(float fov);
		void SetAspectRatio( double aspectRatio);

		float GetNearClipDistance()const { return m_nearClipDistance; }
		float GetFarClipDistance()const { return m_farClipDistance; }
		float GetFOV()const { return m_fov; }

		Kiwi::Matrix4 GetViewMatrix()const { return m_viewMatrix; }
		Kiwi::Matrix4 GetViewMatrix2D()const { return m_viewMatrix2D; }
		Kiwi::Matrix4 GetProjectionMatrix()const { return m_projMatrix; }
		Kiwi::Matrix4 GetOrthoMatrix()const { return m_orthoMatrix; }

	};
};

#endif