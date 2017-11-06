#include "Matrix4.h"

#include <sstream>

namespace Kiwi
{

	Matrix4::Matrix4()
	{

		a1 = a2 = a3 = a4 = 0.0;
		b1 = b2 = b3 = b4 = 0.0;
		c1 = c2 = c3 = c4 = 0.0;
		d1 = d2 = d3 = d4 = 0.0;

	}

	Matrix4 Matrix4::Transpose()const
	{

		Matrix4 kTranspose;
        
		kTranspose.a1 = a1;
		kTranspose.a2 = b1;
		kTranspose.a3 = c1;
		kTranspose.a4 = d1;

		kTranspose.b1 = a2;
		kTranspose.b2 = b2;
		kTranspose.b3 = c2;
		kTranspose.b4 = d2;

		kTranspose.c1 = a3;
		kTranspose.c2 = b3;
		kTranspose.c3 = c3;
		kTranspose.c4 = d3;

		kTranspose.d1 = a4;
		kTranspose.d2 = b4;
		kTranspose.d3 = c4;
		kTranspose.d4 = d4;

        return kTranspose;

	}

	void Matrix4::operator= (const Kiwi::Matrix4& m4)
	{

		a1 = m4.a1;
		a2 = m4.a2;
		a3 = m4.a3;
		a4 = m4.a4;

		b1 = m4.b1;
		b2 = m4.b2;
		b3 = m4.b3;
		b4 = m4.b4;

		c1 = m4.c1;
		c2 = m4.c2;
		c3 = m4.c3;
		c4 = m4.c4;

		d1 = m4.d1;
		d2 = m4.d2;
		d3 = m4.d3;
		d4 = m4.d4;

	}

	Matrix4 Matrix4::operator* (const Matrix4& matrix)const
	{
		/*
		a1 a2 a3 a4
		b1 b2 b3 b4
		c1 c2 c3 c4
		d1 d2 d3 d4 */

		Matrix4 mat;
		mat.a1 = a1 * matrix.a1 + a2 * matrix.b1 + a3 * matrix.c1 + a4 * matrix.d1;
		mat.a2 = a1 * matrix.a2 + a2 * matrix.b2 + a3 * matrix.c2 + a4 * matrix.d2;
		mat.a3 = a1 * matrix.a3 + a2 * matrix.b3 + a3 * matrix.c3 + a4 * matrix.d3;
		mat.a4 = a1 * matrix.a4 + a2 * matrix.b4 + a3 * matrix.c4 + a4 * matrix.d4;

		mat.b1 = b1 * matrix.a1 + b2 * matrix.b1 + b3 * matrix.c1 + b4 * matrix.d1;
		mat.b2 = b1 * matrix.a2 + b2 * matrix.b2 + b3 * matrix.c2 + b4 * matrix.d2;
		mat.b3 = b1 * matrix.a3 + b2 * matrix.b3 + b3 * matrix.c3 + b4 * matrix.d3;
		mat.b4 = b1 * matrix.a4 + b2 * matrix.b4 + b3 * matrix.c4 + b4 * matrix.d4;

		mat.c1 = c1 * matrix.a1 + c2 * matrix.b1 + c3 * matrix.c1 + c4 * matrix.d1;
		mat.c2 = c1 * matrix.a2 + c2 * matrix.b2 + c3 * matrix.c2 + c4 * matrix.d2;
		mat.c3 = c1 * matrix.a3 + c2 * matrix.b3 + c3 * matrix.c3 + c4 * matrix.d3;
		mat.c4 = c1 * matrix.a4 + c2 * matrix.b4 + c3 * matrix.c4 + c4 * matrix.d4;

		mat.d1 = d1 * matrix.a1 + d2 * matrix.b1 + d3 * matrix.c1 + d4 * matrix.d1;
		mat.d2 = d1 * matrix.a2 + d2 * matrix.b2 + d3 * matrix.c2 + d4 * matrix.d2;
		mat.d3 = d1 * matrix.a3 + d2 * matrix.b3 + d3 * matrix.c3 + d4 * matrix.d3;
		mat.d4 = d1 * matrix.a4 + d2 * matrix.b4 + d3 * matrix.c4 + d4 * matrix.d4;
	
		return mat;

	}

	std::wstring Matrix4::ToString()const
	{

		std::wstringstream str;
		str << a1 << ", " << a2 << ", " << a3 << ", " << a4 << "\n";
		str	<< b1 << ", " << b2 << ", " << b3 << ", " << b4 << "\n";
		str	<< c1 << ", " << c2 << ", " << c3 << ", " << c4 << "\n";
		str	<< d1 << ", " << d2 << ", " << d3 << ", " << d4 << "\n";

		return str.str();

	}

	//----------static member functions--------------------/

	Matrix4 Matrix4::Translation(const Kiwi::Vector3d& translation)
	{

		/*
		1 0 0 0
		0 1 0 0
		0 0 1 0
		x y z 1  */

		Matrix4 trans;
		trans.a1 = 1.0;
		trans.b2 = 1.0;
		trans.c3 = 1.0;
		trans.d4 = 1.0;

		trans.d1 = translation.x;
		trans.d2 = translation.y;
		trans.d3 = translation.z;

		return trans;

	}

	Matrix4 Matrix4::Scaling(const Kiwi::Vector3d& scaling)
	{

		/*
		x 0 0 0
		0 y 0 0
		0 0 z 0
		0 0 0 1  */

		Matrix4 scale;
		scale.a1 = scaling.x;
		scale.b2 = scaling.y;
		scale.c3 = scaling.z;
		scale.d4 = 1.0;

		return scale;

	}

	Matrix4 Matrix4::Projection( double fovy, double aspectRatio, double nearView, double farView)
	{

		/*
		Creates the following matrix:
		xScale     0          0               0
		0        yScale       0               0
		0          0       zf/(zf-zn)         0
		0          0       -zn*zf/(zf-zn)     1

		where:
		yScale = cot(fovY/2)
		xScale = yScale / aspect ratio*/

		double fovTan = std::tan( fovy / 2.0 );
		if(fovTan == 0) fovTan = 0.001;
		if(aspectRatio == 0) aspectRatio = 1.0;
		if(farView - nearView == 0) farView = nearView + 1.0;
		double yScale = 1.0 / fovTan;
		double xScale = yScale / aspectRatio;

		Matrix4 proj;

		proj.a1 = xScale;
		proj.b2 = yScale;
		proj.c3 = farView / ( farView-nearView );
		proj.d3 = ( -nearView*farView ) / ( farView-nearView );
		proj.c4 = 1.0;
		
		return proj;

	}

	Matrix4 Matrix4::Orthographic( double screenWidth, double screenHeight, double nearView, double farView)
	{

		/*
		creates the following matrix: zf = farview zn = nearview
		2/w  0    0           0
		0    2/h  0           0
		0    0    1/(zf-zn)   0
		0    0    zn/(zn-zf)  1
		*/

		if(screenWidth <= 0.0) screenWidth = 1.0;
		if(screenHeight <= 0.0) screenHeight = 1.0;

		if(farView - nearView == 0.0) farView = nearView + 1.0;
		if(nearView - farView == 0.0) nearView = farView + 1.0;

		Matrix4 ortho;

		ortho.a1 = 2.0 / screenWidth;
		ortho.b2 = 2.0 / screenHeight;
		ortho.c3 = 1.0 / ( farView - nearView);
		ortho.d3 = nearView / (nearView - farView );
		ortho.d4 = 1.0;

		return ortho;

	}

	Matrix4 Matrix4::OrthographicLH( double screenWidth, double screenHeight, double nearView, double farView )
	{

		/*
		creates the following matrix: zf = farview zn = nearview
		2/w  0    0           0
		0    2/h  0           0
		0    0    1/(zf-zn)   0
		0    0    -zn/(zf-zn)  1
		*/

		if( screenWidth <= 0.0 ) screenWidth = 1.0;
		if( screenHeight <= 0.0 ) screenHeight = 1.0;

		if( farView - nearView == 0.0 ) farView = nearView + 1.0;

		Matrix4 ortho;

		ortho.a1 = 2.0 / screenWidth;
		ortho.b2 = 2.0 / screenHeight;
		ortho.c3 = 1.0 / (farView - nearView);
		ortho.d3 = -nearView / (farView - nearView);
		ortho.d4 = 1.0;

		return ortho;

	}

	Matrix4 Matrix4::OrthographicOffCenterLH( double screenWidth, double screenHeight, double nearView, double farView )
	{

		/*
		creates the following matrix: 
		zf = farview zn = nearview, l = min x value, r = max x value, b = min y value, t = max y value
		2/(r-l)      0            0           0
		0            2/(t-b)      0           0
		0            0            1/(zf-zn)   0
		(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1
		*/

		if( screenWidth <= 0.0 ) screenWidth = 1.0;
		if( screenHeight <= 0.0 ) screenHeight = 1.0;

		if( farView - nearView == 0.0 ) farView = nearView + 1.0;
		if( nearView - farView == 0.0 ) nearView = farView + 1.0;

		Matrix4 ortho;

		ortho.a1 = 2.0 / screenWidth;
		ortho.b2 = 2.0 / screenHeight;
		ortho.c3 = 1.0 / (farView - nearView);
		ortho.c1 = (screenWidth) / (0.0 - screenWidth);
		ortho.c2 = (screenHeight) / (0.0 - screenHeight);
		ortho.d3 = nearView / (nearView - farView);
		ortho.d4 = 1.0;

		return ortho;

	}

};