/******************************************************************************
Copyright (C) 2013 Tomislav Pejsa

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __zhMatrix4_h__
#define __zhMatrix4_h__

#include "zhPrereq.h"
#include "zhVector3.h"

namespace zh
{

enum EulerRotOrder
{
	EulerRotOrder_XYZ = 123,
	EulerRotOrder_XZY = 132,
	EulerRotOrder_YXZ = 213,
	EulerRotOrder_YZX = 231,
	EulerRotOrder_ZXY = 312,
	EulerRotOrder_ZYX = 321
};

/**
* 4x4 matrix class, typically used to represent affine transformations in 3D space.
*/
class Matrix4
{

public:

	float m[4][4]; ///< Matrix components.

	Matrix4(); ///< Constructor. Create an identity matrix.
	Matrix4( float ax, float ay, float az, EulerRotOrder order = EulerRotOrder_YXZ ); ///< Constructor. Create a rotation matrix from Euler angles.
	Matrix4( const Vector3& axis, float angle ); ///< Constructor. Create a rotation matrix from axis-angle rotation.
	Matrix4( const Quat& rot ); ///< Constructor. Create a rotation matrix from quaternion.
	float get( int i, int j ) const; ///< Get an element of the matrix by index.
	void set( int i, int j, float x ); ///< Set an element of the matrix by index.
	Matrix4& identity(); ///< Set matrix to identity.
	Matrix4& translation( float x, float y, float z ); ///< Set matrix to translation matrix.
	Matrix4& translation( const Vector3& trans ); ///< Set current matrix to translation matrix.
	Matrix4& euler( float ax, float ay, float az, EulerRotOrder order = EulerRotOrder_YXZ ); ///< Set current matrix to rotation matrix for Euler angles.
	Matrix4& axisAngle( const Vector3& axis, float angle ); ///< Set current matrix to rotation matrix from axis-angle rotation.
	Matrix4& quat( const Quat& rot ); ///< Set current matrix to rotation matrix from quaternion.
	Matrix4& scale( float sx, float sy, float sz ); ///< Set current matrix to scale matrix.
	Matrix4& scale( const Vector3& scal ); ///< Set current matrix to scale matrix.
	Matrix4& transpose(); ///< Transpose the matrix.
	Matrix4 getTranspose() const; ///< Get transpose of the current matrix.
	float trace() const; ///< Get matrix trace.
	float determinant() const; ///< Get matrix determinant.
	Matrix4& invert(); ///< Invert the matrix.
	Matrix4 getInverse() const; ///< Get inverse of the current matrix.
	bool operator ==( const Matrix4& mat ) const; ///< Determine if matrices are equal.
	bool operator !=( const Matrix4& mat ) const; ///< Determine if matrices are not equal.
	Matrix4 operator*( const Matrix4& mat ) const; ///< Multiply two matrices.
	void operator *=( const Matrix4& mat ); ///< Multiply two matrices.
	Vector3 operator*( const Vector3& v ) const; ///< Multiply the matrix with a vector (upper-left 3x3 submatrix is used).
	bool equals( const Matrix4& mat ) const; ///< Determine if matrices are equal.
	Matrix4 mul( const Matrix4& mat ) const; ///< Multiply two matrices.
	Vector3 mul( const Vector3& v ) const; ///< Multiply the current matrix with a vector (upper-left 3x3 submatrix is used).
	void getEuler( float& ax, float& ay, float& az, EulerRotOrder order = EulerRotOrder_YXZ ) const; ///< Extract Euler angles from the current matrix.
	Matrix4& setEuler( float ax, float ay, float az, EulerRotOrder order = EulerRotOrder_YXZ ); ///< Set current matrix rotation from Euler angles.
	void getAxisAngle( Vector3& axis, float& angle ) const; ///< Extract axis-angle rotation from the current matrix.
	Matrix4& setAxisAngle( const Vector3& axis, float angle ); ///< Set current matrix rotation from axis-angle rotation.
	Quat getQuat() const; ///< Extract quaternion rotation from the current matrix.
	Matrix4& setQuat( const Quat& rot ); ///< Set current matrix rotation from quaternion rotation.
	Matrix4 getRotationMatrix() const; ///< Extract rotation matrix from the current matrix.
	void setRotationMatrix( const Matrix4& mat ); ///< Set current matrix rotation from rotation matrix.
	void getTranslation( float& x, float& y, float& z ) const; ///< Extract translation from the current matrix.
	Matrix4& setTranslation( float x, float y, float z ); ///< Set current matrix translation.
	Vector3 getTranslation() const; ///< Extract translation from the current matrix.
	Matrix4& setTranslation( const Vector3& trans ); ///< Set current matrix translation.
	void getScale( float& sx, float& sy, float& sz ) const; ///< Extract scale from the current matrix.
	Matrix4& setScale( float sx, float sy, float sz ); ///< Set current matrix scale.
	Vector3 getScale() const; ///< Extract scale from the current matrix.
	Matrix4& setScale( const Vector3& scal ); ///< Set current matrix scale.

	static const Matrix4 Identity;

private:

	float determinant3( const float* mat ) const; ///< Get determinant of a 3x3 matrix.
	void getSubMatrix3x3( int i, int j, float* subMat ) const; ///< Get a 3x3 submatrix of the current matrix.
};

}

#endif // __zhMatrix4_h__
