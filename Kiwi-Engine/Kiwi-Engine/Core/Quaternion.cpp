#include "Quaternion.h"

#include <sstream>

namespace Kiwi
{

	/*creates the identity quaternion*/
	Quaternion::Quaternion()
	{
		x = y = z = 0.0f;
		w = 1.0f;

	}

	/*creates a quaternion initialized to the given values*/
	Quaternion::Quaternion(float w, float x, float y, float z)
	{
		this->w = w;
		this->x = x;
		this->y = y;
		this->z = z;

		this->_Normalize();

	}

	Quaternion::Quaternion(const Kiwi::Vector3& axis, float angle)
	{

		angle = angle / 2.0f;

		float sinAngle = std::sin(angle);

		w = std::cos(angle);
		x = axis.x * sinAngle;
		y = axis.y * sinAngle;
		z = axis.z * sinAngle;

		this->_Normalize();

	}

	Quaternion::Quaternion(const Quaternion& otherQuat)
	{

		w = otherQuat.w;
		x = otherQuat.x;
		y = otherQuat.y;
		z = otherQuat.z;

		this->_Normalize();

	}

	Quaternion::~Quaternion()
	{

	}

	void Quaternion::Set(float w, float x, float y, float z)
	{

		this->w = w;
		this->x = x;
		this->y = y;
		this->z = z;

		this->_Normalize();

	}

	void Quaternion::Set(const Kiwi::Vector3& axis, float angle)
	{

		angle = angle / 2.0f;

		float sinAngle = std::sin(angle);

		w = std::cos(angle);
		x = axis.x * sinAngle;
		y = axis.y * sinAngle;
		z = axis.z * sinAngle;

		this->_Normalize();

	}

	Quaternion Quaternion::Scaled(float scale)const
	{

		return Quaternion( w * scale, x * scale, y * scale, z * scale);

	}

	Quaternion Quaternion::Sum(const Quaternion& otherQuat)const
	{

		return Quaternion(w + otherQuat.w, x + otherQuat.x, y + otherQuat.y, z + otherQuat.z);

	}

	Quaternion Quaternion::Cross(const Quaternion& quat)const
	{

		float w2 = quat.w;
		float x2 = quat.x;
		float y2 = quat.y;
		float z2 = quat.z;

		Quaternion newQuat;

		float qw = (w*w2) - (x*x2) - (y*y2) - (z*z2);
		float qx = (w*x2) + (x*w2) + (y*z2) - (z*y2);
		float qy = (w*y2) - (x*z2) + (y*w2) + (z*x2);
		float qz = (w*z2) + (x*y2) - (y*x2) + (z*w2);

		newQuat.Set(qw, qx, qy, qz);

		return newQuat;

	}

	/*normalizes the quaternion*/
	Quaternion Quaternion::Normalized()const
	{
		
		float norm = this->Norm();

		if(norm != 0.0f)
		{
			return Quaternion( w / norm, x / norm,	y / norm, z / norm);
		}else
		{
			return Quaternion( 0.0f, 0.0f, 0.0f, 0.0f );
		}
		
	}

	/*conjugates the quaternion*/
	Quaternion Quaternion::Conjugate()const
	{

		return Quaternion( w, x * -1.0f, y * -1.0f, z * -1.0f);

	}

	Quaternion Quaternion::Inverse()const
	{

		Quaternion quat(this->Conjugate());

		float norm = this->Norm();

		if( norm != 0.0f)
		{
			return quat.Scaled( ( 1.0f / (norm*norm) ) );
		}else
		{
			return Quaternion( 0.0f, 0.0f, 0.0f, 0.0f );
		}

	}

	/*returns the dot product of this quaternion and the other one*/
	float Quaternion::Dot(const Quaternion& otherQuat)const
	{

		return ( (w * otherQuat.w) + (x * otherQuat.x) + (y * otherQuat.y) + (z * otherQuat.z) );

	}

	Kiwi::Vector3 Quaternion::RotatePoint(const Kiwi::Vector3& point)const
	{

		//put the point into a quaternion to do the rotation
		Quaternion vq(0.0f, point.x, point.y, point.z);

		Quaternion qr = this->Cross(vq);
		qr = qr.Cross(this->Conjugate());

		return Kiwi::Vector3(qr.x, qr.y, qr.z);

	}

	//returns the norm of the quaternion
	float Quaternion::Norm()const
	{

		float total = w*w + x*x + y*y + z*z;
		return std::sqrtf( total );

	}

	/*returns a vector representing the euler angles (in radians) that make up this quaternion*/
	Kiwi::Vector3 Quaternion::GetEulerAngles()const
	{

		Kiwi::Vector3 eulerAngles(0.0f, 0.0f, 0.0f);

		//p0 is the quaternion constant (w)
		//p1-3 represent the remaining components of the quaternion *in rotational order*
		//we rotate z-x-y so p1=z p2=x p3=y
		float p0 = w, p1 = z, p2 = x, p3 = y;

		/*multiplier, either 1 or -1, based on rotational order. possible values:
		XYZ = YZX = ZXY = -1
		ZYX = XZY = YXZ = 1*/
		float e = -1.0f;

		//calculations used to find euler angles for arbitrary rotation order (based on value of 'e')
		float r31 = 2.0f * ( p0*p3 - (e*p1*p2) );
		float r33 = 1.0f - ( 2.0f * ( p2*p2 + p3*p3) );
		float r23 = 2.0f * ( p0*p2 + (e*p1*p3) );
		float r12 = 1.0f - ( 2.0f * ( p1*p1 + p2*p2 ) );
		float r13 = 2.0f * ( p0*p1 - (e*p2*p3) );

		//lock x rotation to 89 degrees because singularity occurs at +-90 degrees
		//asin(0.999847) == 88.9977 degrees
		if(r23 >= 0.999847f)
		{
			r23 = 0.999847f;
		}else if(r23 <= -0.999847f)
		{
			r23 = -0.999847f;
		}
		
		eulerAngles.x = std::asin( r23 );

		eulerAngles.y = std::atan2( r31, r33 );
		eulerAngles.z = std::atan2( r13, r12);

		return eulerAngles;

	}

	Kiwi::Matrix4 Quaternion::ToRotationMatrix()const
	{
		
		Kiwi::Matrix4 rotationMatrix;
		
		rotationMatrix.a1 = 1.0f - ( 2.0f * ( y*y + z*z ) );
		rotationMatrix.a2 = 2.0f * ( x*y + w*z );
		rotationMatrix.a3 = 2.0f * ( x*z - w*y );

		rotationMatrix.b1 = 2.0f * ( x*y - w*z );
		rotationMatrix.b2 = 1.0f - ( 2.0f * ( x*x + z*z ) );
		rotationMatrix.b3 = 2.0f * ( y*z + w*x );
		
		rotationMatrix.c1 = 2.0f * ( x*z + w*y );
		rotationMatrix.c2 = 2.0f * ( y*z - w*x );
		rotationMatrix.c3 = 1.0f - ( 2.0f * ( x*x + y*y ) );

		rotationMatrix.d4 = 1.0f;
		
		return rotationMatrix;

	}

	std::wstring Quaternion::ToString()const
	{

		std::wstringstream str;
		str << "w:" << w << ", x:" << x << ", y:" << y << ", z:" << z;
		return str.str();

	}

	Quaternion Quaternion::operator* (const Quaternion& quat)const
	{

		float w2 = quat.w;
		float x2 = quat.x;
		float y2 = quat.y;
		float z2 = quat.z;

		Quaternion newQuat;

		float qw = (w*w2) - (x*x2) - (y*y2) - (z*z2);
		float qx = (w*x2) + (x*w2) + (y*z2) - (z*y2);
		float qy = (w*y2) - (x*z2) + (y*w2) + (z*x2);
		float qz = (w*z2) + (x*y2) - (y*x2) + (z*w2);

		newQuat.Set(qw, qx, qy, qz);

		return newQuat;

	}

	void Quaternion::operator= (const Quaternion& quat)
	{
		w = quat.w;
		x = quat.x;
		y = quat.y;
		z = quat.z;

		this->_Normalize();
	}

	bool Quaternion::operator== (const Quaternion& quat)const
	{

		return ( w == quat.w && x == quat.x && y == quat.y && z == quat.z );

	}

	void Quaternion::_Normalize()
	{

		float norm = this->Norm();

		if(norm != 0.0f)
		{
			w /= norm;
			x /= norm;	
			y /= norm; 
			z /= norm;
		}

	}


	//static member functions-------------------------

	/*returns a quaternion created by:
	1. rotating about the z-axis by vec.z radians
	2. rotating about the x-axis by vec.x radians
	3. rotating about the y-axis by vec.y radians*/
	Quaternion Quaternion::FromEuler(const Kiwi::Vector3& vec)
	{

		//create the rotations
		Quaternion xRot(Kiwi::Vector3::right(), vec.x);
		Quaternion yRot(Kiwi::Vector3::up(), vec.y);
		Quaternion zRot(Kiwi::Vector3::forward(), vec.z);

		//multiply in ZXY order
		Quaternion finalRot;
		finalRot = zRot.Cross(finalRot);
		finalRot = xRot.Cross(finalRot);
		finalRot = yRot.Cross(finalRot);
		//finalRot = yRot.Cross(xRot.Cross(zRot));

		//finalRot = zRot * xRot * yRot;
		//finalRot = (yRot * (xRot * zRot));

		return finalRot;

	}

	/*Returns a quaternion q such that v1*q = v2; the quaternion that rotates v1 to v2
	Both v1 and v2 are assumed to be normalized*/
	Quaternion Quaternion::RotateTo(const Kiwi::Vector3& v1, const Kiwi::Vector3& v2)
	{

		Quaternion q;

		Kiwi::Vector3 v = v1.Cross(v2);

		q.x = v.x;
		q.y = v.y;
		q.z = v.z;

		/*only works if v1 and v2 are normalized, if not then have to use
		q.w = sqrt(v1.length_squared() * v2.length_squared()) + dot(v1,v2)*/
		q.w = 1.0f + v1.Dot(v2);

		//q = q.Normalized();

		return q;

	}

};