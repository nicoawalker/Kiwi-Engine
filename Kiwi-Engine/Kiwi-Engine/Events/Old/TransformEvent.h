#ifndef _KIWI_TRANSFORMEVENT_H_
#define _KIWI_TRANSFORMEVENT_H_

#include "Event.h"

#include "..\Core\Quaternion.h"
#include "..\Core\Vector3d.h"

#include <Windows.h>

namespace Kiwi
{

	class Transform;

	class TransformEvent: 
		public Kiwi::Event<Kiwi::Transform>
	{
	public:

		enum TRANSFORMEVENT_TYPE { TRANSFORM_ROTATION, TRANSFORM_TRANSLATION };

	protected:

		TRANSFORMEVENT_TYPE m_transformEventType;

		Kiwi::Quaternion m_oldRotation;
		Kiwi::Quaternion m_rotation;
		double m_rotationRadians;
		Kiwi::Vector3d m_rotationOrigin;
		Kiwi::Transform* m_originTransform;

		Kiwi::Vector3d m_oldPosition;
		Kiwi::Vector3d m_translation;


	public:

		TransformEvent( Kiwi::Transform* source, TRANSFORMEVENT_TYPE eventType, double rotationRadians, Kiwi::Vector3d rotationOrigin, Kiwi::Quaternion rotation, Kiwi::Quaternion oldRotation ):
			Event(source),
			m_transformEventType(eventType), m_rotationRadians(rotationRadians), m_rotationOrigin(rotationOrigin), m_oldRotation(oldRotation), m_rotation(rotation) {}

		TransformEvent( Kiwi::Transform* source, TRANSFORMEVENT_TYPE eventType, double rotationRadians, Kiwi::Transform* originTransform, Kiwi::Quaternion rotation, Kiwi::Quaternion oldRotation ) :
			Event( source ),
			m_transformEventType( eventType ), m_rotationRadians( rotationRadians ), m_originTransform(originTransform), m_oldRotation( oldRotation ), m_rotation( rotation ){}
		
		TransformEvent( Kiwi::Transform* source, TRANSFORMEVENT_TYPE eventType, Kiwi::Vector3d translation, Kiwi::Vector3d oldPosition ) :
			Event( source ),
			m_transformEventType( eventType ), m_oldPosition( oldPosition ), m_translation( translation ){}

		virtual ~TransformEvent() {}

		TRANSFORMEVENT_TYPE GetEventType()const { return m_transformEventType; }

		/*returns a quaternion containing the rotation of the source transform*/
		Kiwi::Quaternion GetRotation()const { return m_rotation; }

		/*returns the rotation the source transform had before being rotated*/
		Kiwi::Quaternion GetLastRotation()const { return m_oldRotation; }

		/*returns the point around which the rotation occured*/
		Kiwi::Vector3d GetRotationOrigin()const { return m_rotationOrigin; }

		/*if the transform was rotated around another transform this will return a pointer to that transform*/
		Kiwi::Transform* GetOriginTransform()const { return m_originTransform; }

		/*returns the rotation angle in radians for a rotation event*/
		double GetRadians()const { return m_rotationRadians; }

		Kiwi::Vector3d GetTranslation()const { return m_translation; }
		Kiwi::Vector3d GetLastPosition()const { return m_oldPosition; }

	};
};

#endif