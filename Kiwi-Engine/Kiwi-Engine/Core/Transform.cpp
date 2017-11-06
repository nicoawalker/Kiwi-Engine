#include "Transform.h"
#include "Math.h"
#include "Entity.h"

#include "../Physics/Rigidbody.h"

namespace Kiwi
{

	Transform::Transform( Kiwi::Scene& scene, Kiwi::Entity& entity ) :
		Kiwi::Component( L"Transform", Kiwi::ComponentType::Transform, scene )
	{
		m_scale = Kiwi::Vector3d( 1.0, 1.0, 1.0 );
		m_lockYaw = m_lockPitch = m_lockRoll = false;
		m_rigidbody = 0;
		m_entity = &entity;
	}

	Transform::~Transform()
	{
	}

	void Transform::_Translate( const Kiwi::Vector3d& translation )
	{
		std::shared_ptr<Kiwi::TransformEvent> tEvent = std::make_shared<Kiwi::TransformEvent>( *this, Kiwi::TransformType::TRANSFORM_TRANSLATION, translation, m_localPosition );

		m_localPosition += translation;
		m_globalPosition += translation;

		this->EmitTransformEvent( tEvent );
	}

	void Transform::_SetPosition( const Kiwi::Vector3d& position )
	{
		Kiwi::Vector3d translation = position - m_localPosition;

		std::shared_ptr<Kiwi::TransformEvent> tEvent = std::make_shared<Kiwi::TransformEvent>( *this, Kiwi::TransformType::TRANSFORM_TRANSLATION, translation, m_localPosition );

		m_localPosition += translation;
		m_globalPosition += translation;

		this->EmitTransformEvent( tEvent );
	}

	void Transform::_SetGlobalPosition( const Kiwi::Vector3d& position )
	{
		m_globalPosition = position;

		Kiwi::Vector3d translation = position - m_globalPosition;

		if( m_entity != nullptr && m_entity->GetParent() != nullptr )
		{
			Kiwi::Transform* parentTransform = m_entity->GetParent()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
			if( parentTransform != 0 )
			{
				m_localPosition = m_globalPosition - parentTransform->GetGlobalPosition();

			} else
			{
				m_localPosition = m_globalPosition;
			}

		} else
		{
			m_localPosition = m_globalPosition;
		}

		std::shared_ptr<Kiwi::TransformEvent> tEvent = std::make_shared<Kiwi::TransformEvent>( *this, Kiwi::TransformType::TRANSFORM_TRANSLATION, translation, m_localPosition );

		this->EmitTransformEvent( tEvent );
	}

	void Transform::Update()
	{
		if( m_entity != 0 && m_entity->GetParent() != 0 && m_entity->GetParent()->GetType() == m_entity->GetType() )
		{
			Kiwi::Transform* parentTransform = m_entity->GetParent()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );

			/*update global position based on parent's global position*/
			m_globalPosition = m_localPosition + parentTransform->GetGlobalPosition();

			/*rotate this transform around the center of the parent transform whenever the parent transform is rotated to maintain the relative position*/
			static Quaternion lastParentRot;
			if( parentTransform->GetRotation() != lastParentRot )
			{
				Kiwi::Quaternion diff = parentTransform->GetRotation() - lastParentRot;
				lastParentRot = parentTransform->GetRotation();

				Kiwi::Vector3d newPos = diff.RotatePoint( m_localPosition );
				m_globalPosition += newPos - m_localPosition;
				m_localPosition = newPos;
			}

		} else
		{
			m_globalPosition = m_localPosition;
		}
	}

	void Transform::Translate( double x, double y, double z )
	{
		this->_Translate( Kiwi::Vector3d( x, y, z ) );
	}

	void Transform::Translate( const Kiwi::Vector3& translation )
	{
		this->_Translate( Kiwi::Vector3d( (double)translation.x, (double)translation.y, (double)translation.z ) );
	}

	void Transform::Translate( const Kiwi::Vector3d& translation )
	{
		this->_Translate( translation );
	}

	void Transform::SetPosition( const Kiwi::Vector2d& newPosition )
	{
		this->_SetPosition( Kiwi::Vector3d( newPosition.x, newPosition.y, m_localPosition.z ) );
	}

	void Transform::SetPosition( const Kiwi::Vector3& newPosition )
	{
		this->_SetPosition( Kiwi::Vector3d( (double)newPosition.x, (double)newPosition.y, (double)newPosition.z ) );
	}

	void Transform::SetPosition( const Kiwi::Vector3d& newPosition )
	{
		this->_SetPosition( newPosition );
	}

	void Transform::SetPosition( const Kiwi::Transform& transform )
	{
		this->_SetPosition( transform.GetPosition() );
	}

	void Transform::SetGlobalPosition( double x, double y, double z )
	{
		this->_SetGlobalPosition( Kiwi::Vector3d( x, y, z ) );
	}

	void Transform::SetGlobalPosition( const Kiwi::Vector3d& position )
	{
		this->_SetGlobalPosition( position );
	}

	void Transform::Rotate(const Kiwi::Vector3d& rotationAxis, double rotationAmount)
	{
		Kiwi::Quaternion rotQuat(rotationAxis, rotationAmount);

		this->Rotate(rotQuat);
	}

	void Transform::Rotate(const Kiwi::Quaternion& rotation)
	{
		std::shared_ptr<Kiwi::TransformEvent> rEvent = std::make_shared<Kiwi::TransformEvent>( *this, Kiwi::TransformType::TRANSFORM_ROTATION, 0.0, *this, rotation, m_rotation );

		//multiply the current rotation with the new rotation to get the final rotation
		m_rotation = rotation.Cross(m_rotation).Normalized();

		this->EmitTransformEvent( rEvent );

		//if an axis is locked, we need to rotate that axis back to its fixed rotation value
		if(m_lockRoll)
		{
			Kiwi::Vector3d eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.z != m_lockPosition.z)
			{
				Kiwi::Quaternion zRot( this->GetForward(), -(eulerAngles.z - m_lockPosition.z) );

				//Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, -(eulerAngles.z - m_lockPosition.z), this, zRot, m_rotation );

				m_rotation = zRot.Cross(m_rotation).Normalized();

				//this->BroadcastTransformEvent( rotEvent );
			}
		}

		if(m_lockPitch)
		{
			Kiwi::Vector3d eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.x != m_lockPosition.x)
			{
				Kiwi::Quaternion xRot( this->GetRight(), -(eulerAngles.x - m_lockPosition.x) );

				//Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, -(eulerAngles.x - m_lockPosition.x), this, xRot, m_rotation );

				m_rotation = xRot.Cross(m_rotation).Normalized();

				//this->BroadcastTransformEvent( rotEvent );
			}
		}

		if(m_lockYaw)
		{
			Kiwi::Vector3d eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.y != m_lockPosition.y)
			{
				Kiwi::Quaternion yRot( this->GetUp(), -(eulerAngles.y - m_lockPosition.y) );

				//Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, -(eulerAngles.y - m_lockPosition.y), this, yRot, m_rotation );

				m_rotation = yRot.Cross(m_rotation).Normalized();

				//this->BroadcastTransformEvent( rotEvent );
			}
		}
	}

	void Transform::RotateAround( const Kiwi::Vector3& point, const Kiwi::Quaternion& rotation )
	{
		this->RotateAround( Kiwi::Vector3d( point ), rotation );
	}

	void Transform::RotateAround( const Kiwi::Vector3d& point, const Kiwi::Quaternion& rotation )
	{
		/*translate to do the rotation in local space*/
		Kiwi::Vector3d local = m_globalPosition - point;
		Kiwi::Vector3d aroundOrigin = rotation.RotatePoint( local );

		/*set the position to be the sum of the local rotation and the position being rotated around*/
		this->SetGlobalPosition( aroundOrigin + point );
	}

	void Transform::SetRotation( const Kiwi::Quaternion& newRotation )
	{
		//calculate the difference between the new rotation and the old
		/*
		diff * q1 = q2  --->  diff = q2 * inverse(q1)
		where:  inverse(q1) = conjugate(q1) / abs(q1)
		and:  conjugate( quaternion(re, i, j, k) ) = quaternion(re, -i, -j, -k)
		*/
		Kiwi::Quaternion diff = newRotation.Cross( m_rotation.Inverse() ).Normalized();

		std::shared_ptr<Kiwi::TransformEvent> rEvent = std::make_shared<Kiwi::TransformEvent>( *this, Kiwi::TransformType::TRANSFORM_ROTATION, 0.0, *this, diff, m_rotation );

		m_rotation = newRotation;

		this->EmitTransformEvent( rEvent );
	}

	void Transform::SetRotation( const Kiwi::Transform& transform )
	{
		this->SetRotation( transform.GetRotation() );
	}

	void Transform::RotateTowards(const Kiwi::Vector3d& target, const Kiwi::Vector3d& up, double maxRotation)
	{
		if(m_localPosition == target) return;

		Kiwi::Vector3d forwardDifference = (target - m_localPosition).Normalized();
		Kiwi::Vector3d forward = Kiwi::Vector3d::forward();

		Kiwi::Vector3d rotationAxis = forward.Cross( forwardDifference ).Normalized();

		double dot = forward.Dot(forwardDifference);
		if( std::abs(dot + 1.0) < 0.000001 )
		{
			//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
			this->Rotate(up, KIWI_PI);
			return;

		}else if( std::abs(dot - 1.0) < 0.000001 )
		{
			//already facing each other, do nothing
			return;
		}

		double rotationAngle = std::acos( dot );

		Kiwi::Quaternion rotation( rotationAxis, rotationAngle );
		m_rotation = rotation;

		double zAngle = Kiwi::Vector3d::right().Dot( this->GetRight() );
		if( zAngle != 0.0 )
		{
			this->Rotate( this->GetForward(), -zAngle );
		}

		/*Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

		if(eulerAngles.z != 0.0)
		{
			this->Rotate(this->GetForward(),-eulerAngles.z); 
		}*/

		//calculate the vector between the current position and the target
		//Kiwi::Vector3d vectorBetween = (target - m_localPosition).Normalized();

		//Kiwi::Vector3d forward = this->GetForward();
		//Kiwi::Vector3d up = this->GetUp();

		//double dot = forward.Dot(vectorBetween);

		//if( dot < -0.99998 )
		//{
		//	//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
		//	this->Rotate(up, KIWI_PI);
		//	return;

		//}else if( dot > 0.999998 )
		//{
		//	//already facing each other, do nothing
		//	return;
		//}

		//Kiwi::Vector3d rotationAxis = forward.Cross(vectorBetween);

		//double angle = std::acos( dot );
		////angle = ToDegrees(angle);

		//if( maxRotation > 0.000001 && angle > maxRotation )
		//{
		//	angle = maxRotation;
		//}

		//Kiwi::Quaternion rotation( rotationAxis, angle );

		//this->Rotate(rotationAxis, angle);	

		////now the quaternion is facing the target, but it may be rotated around the z-axis
		////need to rotate around the z-axis to the desired up direction
		//Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

		//if(eulerAngles.z != 0.0)
		//{
		//	this->Rotate(this->GetForward(),-eulerAngles.z); 
		//}
	}

	void Transform::RotateTowards(const Transform& target, const Kiwi::Vector3d& up, double maxRotation)
	{
		this->RotateTowards(target.GetGlobalPosition(), up, maxRotation);
	}

	void Transform::RotateAway(const Kiwi::Vector3d& target, const Kiwi::Vector3d& up, double maxRotation)
	{
		if(m_localPosition == target) return;

		//calculate the vector between the current position and the target
		Kiwi::Vector3d vectorBetween(target.x - m_localPosition.x, target.y - m_localPosition.y, target.z - m_localPosition.z);
		vectorBetween = vectorBetween.Normalized();

		Kiwi::Vector3d forward = this->GetForward().Scaled(-1.0);

		double dot = forward.Dot(vectorBetween);

		if( dot < -0.99998 )
		{

			//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
			this->Rotate(up, KIWI_PI);
			return;

		}else if( dot > 0.99998 )
		{
			//already facing each other, do nothing
			return;

		}

		Kiwi::Vector3d rotationAxis = forward.Cross(vectorBetween);

		double angle = std::acos( forward.Dot(vectorBetween) );

		if(maxRotation != 0.0 && angle > maxRotation)
		{
			angle = maxRotation;
		}

		this->Rotate(rotationAxis, angle);	

		//now the quaternion is facing the target, but it may be rotated around the z-axis
		//need to rotate around the z-axis to the desired up direction
		Kiwi::Vector3d eulerAngles = m_rotation.GetEulerAngles();

		if(eulerAngles.z != 0.0)
		{
			this->Rotate(this->GetForward(),-eulerAngles.z); 
		}
	}

	void Transform::RotateAway(const Transform& target, const Kiwi::Vector3d& up, double maxRotation)
	{
		this->RotateAway(target.GetGlobalPosition(), up, maxRotation);
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
	void Transform::LockAxisTo(Kiwi::AXIS axis, double radians)
	{
		switch(axis)
		{
		case Kiwi::AXIS::X_AXIS:
			{
				m_lockPitch = true;
				m_lockPosition.x = radians;

				Kiwi::Vector3d eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.x != m_lockPosition.x)
				{
					Kiwi::Quaternion xRot( this->GetRight(), -(eulerAngles.x - m_lockPosition.x) );
					m_rotation = xRot.Cross(m_rotation);
				}

				break;
			}
		case Kiwi::AXIS::Y_AXIS:
			{
				m_lockYaw = true;
				m_lockPosition.y = radians;

				Kiwi::Vector3d eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.y != m_lockPosition.y)
				{
					Kiwi::Quaternion yRot( this->GetUp(), -(eulerAngles.y - m_lockPosition.y) );
					m_rotation = yRot.Cross(m_rotation);
				}

				break;
			}
		case Kiwi::AXIS::Z_AXIS:
			{
				m_lockRoll = true;
				m_lockPosition.z = radians;

				Kiwi::Vector3d eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.z != m_lockPosition.z)
				{
					Kiwi::Quaternion zRot( this->GetForward(), -(eulerAngles.z - m_lockPosition.z) );
					m_rotation = zRot.Cross(m_rotation);
				}

				break;
			}
		default:break;
		}
	}

	/*returns the distance between the two transforms*/
	double Transform::GetDistance(Kiwi::Transform* targetTransform)
	{
		return std::sqrt( this->GetSquareDistance(targetTransform) );
	}

	/*returns the distance between this transform and the target vector*/
	double Transform::GetDistance( const Kiwi::Vector3d& target )
	{
		return std::sqrt( this->GetSquareDistance( target ) );
	}

	/*returns the square of the distance between the two transforms*/
	double Transform::GetSquareDistance( Kiwi::Transform* targetTransform )
	{
		if( targetTransform == 0 )
		{
			return 0.0f;
		}

		return ( std::pow( (targetTransform->GetGlobalPosition().x - m_globalPosition.x), 2) +
				 std::pow( (targetTransform->GetGlobalPosition().y - m_globalPosition.y), 2) +
				 std::pow( (targetTransform->GetGlobalPosition().z - m_globalPosition.z), 2) );
	}

	/*returns the square of the distance between this transform and the target vector*/
	double Transform::GetSquareDistance( const Kiwi::Vector3d& target )
	{
		return ( std::pow( (target.x - m_globalPosition.x), 2 ) +
				 std::pow( (target.y - m_globalPosition.y), 2 ) +
				 std::pow( (target.z - m_globalPosition.z), 2 ) );
	}

	Kiwi::Matrix4 Transform::GetWorldMatrix()
	{
		/*create the translation matrix. if the object is 2D we dont use the z coordinate to make the matrix*/
		Kiwi::Matrix4 translate = (m_entity->GetType() == Kiwi::Entity::ENTITY_2D) ? Kiwi::Matrix4::Translation( Kiwi::Vector3d( m_globalPosition.x, m_globalPosition.y, 0.0 ) ) : Kiwi::Matrix4::Translation( m_globalPosition );
		Kiwi::Matrix4 scale = Kiwi::Matrix4::Scaling(m_scale);
		Kiwi::Matrix4 rotate = m_rotation.ToRotationMatrix();
		
		/* using row-major matrices so to get proper scaling when rotating need to use scale*rotate*translate
		if using column major matrices, need to use rotate*scale*translate */
		Kiwi::Matrix4 world = (scale * rotate) * translate;
		return world;
	}

	Kiwi::Vector3d Transform::GetForward()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3d::forward()).Normalized(); 
	}

	Kiwi::Vector3d Transform::GetUp()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3d::up()).Normalized();
	}

	Kiwi::Vector3d Transform::GetRight()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3d::right()).Normalized();
	}

};