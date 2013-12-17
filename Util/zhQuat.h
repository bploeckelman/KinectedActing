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

#ifndef __zhQuat_h__
#define __zhQuat_h__

#include "zhPrereq.h"
#include "zhVector3.h"
#include "zhMatrix4.h"

namespace zh
{

/**
* Quaternion class. Typically used to represent rotations.
*/
class Quat
{

public:

	float w; ///< Quaternion w component.
	float x; ///< Quaternion x component.
	float y; ///< Quaternion y component.
	float z; ///< Quaternion z component.

	Quat(); ///< Constructor.
	Quat( float w, float x, float y, float z ); ///< Constructor.
	Quat( float ax, float ay, float az, EulerRotOrder order = EulerRotOrder_YXZ ); ///< Constructor. Create quaternion rotation from Euler angles.
	Quat( const Vector3& axis, float angle ); ///< Constructor. Create quaternion rotation from axis-angle rotation.
	Quat& conjugate(); ///< Conjugate the quaternion.
	Quat getConjugate() const; ///< Get conjugate of the current quaternion.
	float lengthSq() const; ///< Get quaternion length.
	float length() const; ///< Get quaternion length.
	Quat& normalize(); ///< Normalize the quaternion.
	Quat getNormalized() const; ///< Get the current quaternion in normalized form.
	Quat& invert(); ///< Invert the quaternion.
	Quat getInverse() const; ///< Get inverse of the current quaternion.
	Quat exp() const;
	Quat log() const;
	void getEuler( float& ax, float& ay, float& az, EulerRotOrder order = EulerRotOrder_YXZ ) const; ///< Get Euler angles from the current quaternion.
	float getAngle() const; ///< Get angle represented by the current quaternion.
	void getAxisAngle( Vector3& axis, float& angle ) const; ///< Get axis-angle rotation from the current quaternion.
	bool operator ==( const Quat& q ) const; ///< Determine if quaternions are equal.
	bool operator !=( const Quat& q ) const; ///< Determine if quaternions are not equal.
	Quat operator +() const;
	Quat operator -() const; ///< Negate the quaternion.
	Quat operator +( const Quat& q ) const; ///< Add two quaternions.
	void operator +=( const Quat& q ); ///< Add two quaternions.
	Quat operator -( const Quat& q ) const; ///< Subtract two quaternions.
	void operator -=( const Quat& q ); ///< Subtract two quaternions.
	Quat operator *( float s ) const; ///< Scalar-quaternion multiplication.
	void operator *=( float s ); ///< Scalar-quaternion multiplication.
	Quat operator /( float s ) const; ///< Scalar-quaternion division.
	void operator /=( float s ); ///< Scalar-quaternion division.
	Quat operator *( const Quat& q ) const; ///< Quaternion cross-product.
	void operator *=( const Quat& q ); ///< Quaternion cross-product.
	bool equals( const Quat& q ) const; ///< Determine if quaternions are equal.
	Quat& negate(); ///< Negate the quaternion.
	Quat add( const Quat& q ) const; ///< Add two quaternions.
	float dot( const Quat& q ) const; ///< Quaternion dot-product.
	Quat cross( const Quat& q ) const; ///< Quaternion cross-product.
	Quat slerp( const Quat& q, float t ) const; ///< Spherical linear interpolation between quaternions.
	Quat nlerp( const Quat& q, float t ) const; ///< Linear interpolation between quaternions.
	Quat squad( const Quat& qa, const Quat& qb, const Quat& q, float t ) const; ///< Cubic interpolation between quaternions.

	static const Quat Identity;

};

}

#endif // __zhQuat_h__
