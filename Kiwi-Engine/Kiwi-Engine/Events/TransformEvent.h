#ifndef _KIWI_TRANSFORMEVENT_H_
#define _KIWI_TRANSFORMEVENT_H_

#include "IEvent.h"

#include "../Types.h"

#include "..\Core\Quaternion.h"
#include "../Core/Vector3d.h"

#include <Windows.h>

namespace Kiwi
{

	class Transform;

	enum TransformType { TRANSFORM_ROTATION, TRANSFORM_TRANSLATION };

	class TransformEvent :
		public Kiwi::IEvent<Kiwi::Transform>
	{
	public:

		TransformType eventType;

		Kiwi::Quaternion lastRotation;
		Kiwi::Quaternion rotation;
		double rotationRadians;
		Kiwi::Vector3d rotationOrigin;
		Kiwi::Transform* originTransform; //transform representing the origin of a rotation

		Kiwi::Vector3d lastPosition;
		Kiwi::Vector3d translation;


	public:

		TransformEvent( Kiwi::Transform& source, TransformType eventType, double rotationRadians, Kiwi::Vector3d rotationOrigin, Kiwi::Quaternion rotation, Kiwi::Quaternion previousRotation ) :
			IEvent<Kiwi::Transform>( &source, L"Rotation" ),
			eventType( eventType ), rotationRadians( rotationRadians ), rotationOrigin( rotationOrigin ), lastRotation( previousRotation ), rotation( rotation ), originTransform(0)
		{
		}

		TransformEvent( Kiwi::Transform& source, TransformType eventType, double rotationRadians, Kiwi::Transform& originTransform, Kiwi::Quaternion rotation, Kiwi::Quaternion previousRotation ) :
			IEvent<Kiwi::Transform>( &source, L"Rotation" ),
			eventType( eventType ), rotationRadians( rotationRadians ), originTransform( &originTransform ), lastRotation( previousRotation ), rotation( rotation )
		{
		}

		TransformEvent( Kiwi::Transform& source, TransformType eventType, Kiwi::Vector3d translation, Kiwi::Vector3d lastPosition ) :
			IEvent<Kiwi::Transform>( &source, L"Translation" ),
			eventType( eventType ), lastPosition( lastPosition ), translation( translation ), originTransform(0)
		{
		}

		virtual ~TransformEvent() {}

	};
};

#endif