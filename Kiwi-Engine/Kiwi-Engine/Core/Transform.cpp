#include "Transform.h"
#include "Math.h"

namespace Kiwi
{

	Transform::Transform()
	{

		m_scale = Kiwi::Vector3( 1.0f, 1.0f, 1.0f );

		m_lockYaw = m_lockPitch = m_lockRoll = false;

	}

	void Transform::Rotate(const Kiwi::Vector3& rotationAxis, float rotationAmount)
	{

		Kiwi::Quaternion rotQuat(rotationAxis, rotationAmount);

		this->Rotate(rotQuat);

	}

	void Transform::Rotate(const Kiwi::Quaternion& rotation)
	{

		//multiply the current rotation with the new rotation to get the final rotation
		m_rotation = rotation.Cross(m_rotation);

		//if an axis is locked, we need to rotate that axis back to its fixed rotation value
		if(m_lockRoll)
		{

			Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.z != m_lockPosition.z)
			{
				Kiwi::Quaternion zRot(this->GetForward(),-(eulerAngles.z - m_lockPosition.z)); 
				m_rotation = zRot.Cross(m_rotation);
			}

		}

		if(m_lockPitch)
		{

			Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.x != m_lockPosition.x)
			{
				Kiwi::Quaternion xRot(this->GetRight(),-(eulerAngles.x - m_lockPosition.x)); 
				m_rotation = xRot.Cross(m_rotation);
			}

		}

		if(m_lockYaw)
		{

			Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.y != m_lockPosition.y)
			{
				Kiwi::Quaternion yRot(this->GetUp(),-(eulerAngles.y - m_lockPosition.y));
				m_rotation = yRot.Cross(m_rotation);
			}

		}

	}

	void Transform::SetRotation( const Kiwi::Quaternion& newRotation )
	{

		m_rotation = newRotation;

	}

	void Transform::SetRotation( const Kiwi::Transform& transform )
	{

		this->SetRotation( transform.GetRotation() );

	}

	void Transform::RotateTowards(const Kiwi::Vector3& target, float maxRotation)
	{

		if(m_position == target) return;

		//calculate the vector between the current position and the target
		Kiwi::Vector3 vectorBetween(target.x - m_position.x, target.y - m_position.y, target.z - m_position.z);
		vectorBetween = vectorBetween.Normalized();

		Kiwi::Vector3 forward = this->GetForward();
		Kiwi::Vector3 up = this->GetUp();

		float dot = forward.Dot(vectorBetween);

		if( dot < -0.999999f )
		{

			//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
			this->Rotate(up, 3.141592f);
			return;

		}else if( dot > 0.9999f )
		{

			//already facing each other, do nothing
			return;

		}

		Kiwi::Vector3 rotationAxis = forward.Cross(vectorBetween);

		float angle = std::acos( forward.Dot(vectorBetween) );
		//angle = ToDegrees(angle);

		if(maxRotation != 0.0f && angle > maxRotation)
		{
			angle = maxRotation;
		}

		this->Rotate(rotationAxis, angle);	

		//now the quaternion is facing the target, but it may be rotated around the z-axis
		//need to rotate around the z-axis to the desired up direction
		Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

		if(eulerAngles.z != 0.0f)
		{
			this->Rotate(this->GetForward(),-eulerAngles.z); 
		}

	}

	void Transform::RotateTowards(const Transform& target, float maxRotation)
	{

		this->RotateTowards(target.GetPosition(), maxRotation);

	}

	void Transform::RotateAway(const Kiwi::Vector3& target, float maxRotation)
	{

		if(m_position == target) return;

		//calculate the vector between the current position and the target
		Kiwi::Vector3 vectorBetween(target.x - m_position.x, target.y - m_position.y, target.z - m_position.z);
		vectorBetween = vectorBetween.Normalized();

		Kiwi::Vector3 forward = this->GetForward().Scaled(-1.0f);
		Kiwi::Vector3 up = this->GetUp();

		float dot = forward.Dot(vectorBetween);

		if( dot < -0.999999f )
		{

			//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
			this->Rotate(up, 3.141592f);
			return;

		}else if( dot > 0.9999f )
		{

			//already facing each other, do nothing
			return;

		}

		Kiwi::Vector3 rotationAxis = forward.Cross(vectorBetween);

		float angle = std::acos( forward.Dot(vectorBetween) );
		//angle = ToDegrees(angle);

		if(maxRotation != 0.0f && angle > maxRotation)
		{
			angle = maxRotation;
		}

		this->Rotate(rotationAxis, angle);	

		//now the quaternion is facing the target, but it may be rotated around the z-axis
		//need to rotate around the z-axis to the desired up direction
		Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

		if(eulerAngles.z != 0.0f)
		{
			this->Rotate(this->GetForward(),-eulerAngles.z); 
		}

	}

	void Transform::RotateAway(const Transform& target, float maxRotation)
	{

		this->RotateAway(target.GetPosition(), maxRotation);

	}

	/*locks or unlocks the axis to the current rotation, depending on the value of 'lock'*/
	void Transform::LockAxis(Kiwi::AXIS axis, bool lock)
	{

		switch(axis)
		{
		case Kiwi::AXIS::X_AXIS:
			{
				m_lockPitch = lock;
				break;
			}
		case Kiwi::AXIS::Y_AXIS:
			{
				m_lockYaw = lock;
				break;
			}
		case Kiwi::AXIS::Z_AXIS:
			{
				m_lockRoll = lock;
				break;
			}
		default:break;
		}

	}

	/*locks the axis to the specified rotation*/
	void Transform::LockAxisTo(Kiwi::AXIS axis, float radians)
	{

		switch(axis)
		{
		case Kiwi::AXIS::X_AXIS:
			{
				m_lockPitch = true;
				m_lockPosition.x = radians;

				Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.x != m_lockPosition.x)
				{
					Kiwi::Quaternion xRot(this->GetRight(),-(eulerAngles.x - m_lockPosition.x)); 
					m_rotation = xRot.Cross(m_rotation);
				}

				break;
			}
		case Kiwi::AXIS::Y_AXIS:
			{
				m_lockYaw = true;
				m_lockPosition.y = radians;

				Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.y != m_lockPosition.y)
				{
					Kiwi::Quaternion yRot(this->GetUp(),-(eulerAngles.y - m_lockPosition.y));
					m_rotation = yRot.Cross(m_rotation);
				}

				break;
			}
		case Kiwi::AXIS::Z_AXIS:
			{
				m_lockRoll = true;
				m_lockPosition.z = radians;

				Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.z != m_lockPosition.z)
				{
					Kiwi::Quaternion zRot(this->GetForward(),-(eulerAngles.z - m_lockPosition.z)); 
					m_rotation = zRot.Cross(m_rotation);
				}

				break;
			}
		default:break;
		}

	}

	/*returns the distance between the two transforms*/
	float Transform::GetDistance(Kiwi::Transform* targetTransform)
	{

		return std::sqrtf( this->GetSquareDistance(targetTransform) );

	}

	/*returns the distance between this transform and the target vector*/
	float Transform::GetDistance( const Kiwi::Vector3& target )
	{

		return std::sqrtf( this->GetSquareDistance( target ) );

	}

	/*returns the square of the distance between the two transforms*/
	float Transform::GetSquareDistance(Kiwi::Transform* targetTransform)
	{

		return ( std::powf( (targetTransform->GetPosition().x - m_position.x), 2) +
				 std::powf( (targetTransform->GetPosition().y - m_position.y), 2) +
				 std::powf( (targetTransform->GetPosition().z - m_position.z), 2) );

	}

	/*returns the square of the distance between this transform and the target vector*/
	float Transform::GetSquareDistance( const Kiwi::Vector3& target )
	{

		return (std::powf( (target.x - m_position.x), 2 ) +
				 std::powf( (target.y - m_position.y), 2 ) +
				 std::powf( (target.z - m_position.z), 2 ));

	}

	Kiwi::Matrix4 Transform::GetWorldMatrix()const
	{

		Kiwi::Matrix4 translate = Kiwi::Matrix4::Translation(m_position);
		Kiwi::Matrix4 scale = Kiwi::Matrix4::Scaling(m_scale);
		Kiwi::Matrix4 rotate = m_rotation.ToRotationMatrix();
		
		Kiwi::Matrix4 world = rotate * scale * translate;
		return world;

	}

	Kiwi::Vector3 Transform::GetForward()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3::forward()); 
	}

	Kiwi::Vector3 Transform::GetUp()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3::up()); 
	}

	Kiwi::Vector3 Transform::GetRight()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3::right()); 
	}

};