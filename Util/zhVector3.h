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

#ifndef __zhVector3_h__
#define __zhVector3_h__

#include "zhPrereq.h"
#include "zhMathMacros.h"
#include "zhString.h"

// C++
#include <cmath>

namespace zh
{

class Matrix4;
class Quat;

/**
* @brief 3D vector class.
*
* 3D vector class. Can be used to represent any of the following:
* - 3D vector
* - point in 3D space
*/
class Vector3
{

public:

	float x; ///< Vector x component.
	float y; ///< Vector x component.
	float z; ///< Vector x component.

private:

	float w; ///< Vector w component (always 1).

public:

	Vector3(); ///< Constructor. Create a nul-vector.
	Vector3( float x, float y, float z ); ///< Constructor.
	unsigned int size() const; ///< Gets the vector size.
	float get( unsigned int i ) const; ///< Gets an element of the vector by index.
	void set( unsigned int i, float x ); ///< Sets an element of the vector by index.
	Vector3& null(); ///< Initializes this vector as a null-vector.
	float lengthSq() const; ///< Get vector length.
	float length() const; ///< Get vector length.
	Vector3& normalize(); ///< Normalize the current vector.
	Vector3 getNormalized() const; ///< Get the current vector in normalized form.
	float distance( const Vector3& v ) const; ///< Compute distance between vectors.
	float distanceSq( const Vector3& v ) const; ///< Compute squared distance between vectors.
	float angle( const Vector3& v ) const; ///< Compute angle between vectors.
	bool operator ==( const Vector3& v ) const; ///< Determine if vectors are equal.
	bool operator !=( const Vector3& v ) const; ///< Determine if vectors are not equal.
	Vector3 operator +() const;
	Vector3 operator -() const; ///< Negate the vector.
	Vector3 operator *( float s ) const; ///< Scalar-vector multiplication.
	void operator *=( float s ); ///< Scalar-vector multiplication.
	Vector3 operator /( float s ) const; ///< Scalar-vector division.
	void operator /=( float s ); ///< Scalar-vector division.
	Vector3 operator +( const Vector3& v ) const; ///< Add two vectors.
	void operator +=( const Vector3& v ); ///< Add two vectors.
	Vector3 operator -( const Vector3& v ) const; ///< Subtract two vectors.
	void operator -=( const Vector3& v ); ///< Subtract two vectors.
	Vector3 operator *( const Vector3& v ) const; ///< Multiply two vectors (actually scale the vector with another vector).
	void operator *=( const Vector3& v ); ///< Multiply two vectors (actually scale the vector with another vector).
	Vector3 operator *( const Matrix4& mat ) const; ///< Multiply the vector with a matrix.
	void operator *=( const Matrix4& mat ); ///< Multiply the vector with a matrix.
	bool equals( const Vector3& v ) const; ///< Determine if vectors are equal.
	Vector3& negate(); ///< Negate the vector.
	Vector3 mul( const float s ) const; ///< Scalar-vector multiplication.
	Vector3 div( const float s ) const; ///< Scalar-vector division.
	Vector3 add( const Vector3& v ) const; ///< Add two vectors.
	Vector3 sub( const Vector3& v ) const; ///< Subtract two vectors.
	float dot( const Vector3& v ) const; ///< Vector dot-product.
	Vector3 cross( const Vector3& v ) const; ///< Vector cross-product.
	Vector3 mul( const Matrix4& mat ) const; ///< Multiply the current vector with a matrix.
	Vector3& rotate( const Quat& rot ); ///< Rotates the current vector with the specified quaternion.
	Vector3 getRotated( const Quat& rot ) const; ///< Gets the current vector rotated with the specified quaternion.
	Quat getRotationTo( const Vector3& v ) const; ///< Gets the rotation between two vectors.
	
	static const Vector3 Null;
	static const Vector3 XAxis;
	static const Vector3 YAxis;
	static const Vector3 ZAxis;
	static const Vector3 UnitScale;
	
};

template <>
inline std::string toString<Vector3>( const Vector3& v )
{
	std::ostringstream oss;

	oss << v.x;
	oss << " ";
	oss << v.y;
	oss << " ";
	oss << v.z;

	return oss.str();
}

template <>
inline Vector3 fromString<Vector3>( const std::string& tstr )
{
	Vector3 v;
	std::istringstream iss(tstr);

	if( iss.eof() )
		return v;
	iss >> v.x;

	if( iss.eof() )
		return v;
	iss >> v.y;

	if( iss.eof() )
		return v;
	iss >> v.z;

	return v;
}

}

#endif // __Vector3_h__
