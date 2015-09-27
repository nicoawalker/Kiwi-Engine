#ifndef _KIWI_TRANSFORM_H_
#define _KIWI_TRANSFORM_H_

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"

namespace Kiwi
{

	enum AXIS { X_AXIS = 0, Y_AXIS, Z_AXIS };

	class Transform
	{
	protected:

		Kiwi::Quaternion m_rotation;

		Kiwi::Vector3 m_position;

		Kiwi::Vector3 m_scale;

		//stores which axis' are locked
		bool m_lockYaw, m_lockPitch, m_lockRoll;
		//stores the rotation value that each axis should be locked to
		Kiwi::Vector3 m_lockPosition;

	public:

		Transform();
		~Transform() {}

		/*update is called once per frame*/
		void Update() {}

		/*fixed update is called on a regular interval, e.g. 60 times per second*/
		void FixedUpdate() {}

		/*translates the object by the given amount in each direction*/
		void Translate(float x, float y, float z) { m_position += Kiwi::Vector3(x, y, z); };
		/*translates the object by the given amount in each direction*/
		void Translate(const Kiwi::Vector3& translation) { m_position += translation; }
		/*sets the position of the object equal to newPosition*/
		void SetPosition(const Kiwi::Vector3& newPosition) { m_position = newPosition; }

		void SetHeight(float height) { m_position.y = height; }

		void SetScale(const Kiwi::Vector3& scale) { m_scale = scale; }

		void Rotate(const Kiwi::Vector3& rotationAxis, float rotationAmount);

		/*rotates the object by the given quaternion*/
		void Rotate(const Kiwi::Quaternion& rotation);

		/*sets the rotation of the object equal to newRotation*/
		void SetRotation( const Kiwi::Quaternion& newRotation );

		/*sets the rotation to that of the given transform's*/
		void SetRotation( const Kiwi::Transform& transform );

		/*rotates the transform so that the local z+ axis passes through the target point*/
		void RotateTowards(const Kiwi::Vector3& target, float maxRotation = 0.0f);

		/*rotates the transform so that the local z+ axis passes through the target transform*/
		void RotateTowards(const Kiwi::Transform& target, float maxRotation = 0.0f);

		void RotateAway(const Kiwi::Vector3& target, float maxRotation = 0.0f);
		void RotateAway(const Kiwi::Transform& target, float maxRotation = 0.0f);

		/*locks or unlocks the axis to the current rotation, depending on the value of 'lock'*/
		void LockAxis(Kiwi::AXIS axis, bool lock);
		/*locks the axis to the specified rotation*/
		void LockAxisTo(Kiwi::AXIS axis, float radians);

		/*returns the distance between the two transforms*/
		float GetDistance(Kiwi::Transform* target);
		/*returns the distance between this transform and the target vector*/
		float GetDistance(const Kiwi::Vector3& target);

		/*returns the square of the distance between the two transforms*/
		float GetSquareDistance(Kiwi::Transform* target);
		/*returns the square of the distance between this transform and the target vector*/
		float GetSquareDistance(const Kiwi::Vector3& target);

		const Kiwi::Vector3& GetPosition()const { return m_position; }
		const Kiwi::Quaternion& GetRotation()const { return m_rotation; }
		const Kiwi::Vector3& GetScale()const { return m_scale; }

		Kiwi::Matrix4 GetWorldMatrix()const;

		Kiwi::Vector3 GetForward()const;
		Kiwi::Vector3 GetUp()const;
		Kiwi::Vector3 GetRight()const;

	};
};

#endif