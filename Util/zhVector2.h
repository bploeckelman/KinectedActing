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

#ifndef __zhVector2_h__
#define __zhVector2_h__

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
* @brief 2D vector class.
*
* 2D vector class. Can be used to represent any of the following:
* - 2D vector
* - point in 2D space
* - 2D translation
* - 2D scale
*/
class Vector2
{

public:

	float x; ///< Vector x component.
	float y; ///< Vector x component.

	Vector2(); ///< Constructor. Create a nul-vector.
	Vector2( float x, float y ); ///< Constructor.
	unsigned int size() const; ///< Gets the vector size.
	float get( unsigned int i ) const; ///< Gets an element of the vector by index.
	void set( unsigned int i, float x ); ///< Sets an element of the vector by index.
	Vector2& null(); ///< Initializes this vector as a null-vector.
	float lengthSq() const; ///< Get vector length.
	float length() const; ///< Get vector length.
	Vector2& normalize(); ///< Normalize the current vector.
	Vector2 getNormalized() const; ///< Get the current vector in normalized form.
	float distance( const Vector2& v ) const; ///< Compute distance between vectors.
	float distanceSq( const Vector2& v ) const; ///< Compute squared distance between vectors.
	float angle( const Vector2& v ) const; ///< Compute angle between vectors.
	bool operator ==( const Vector2& v ) const; ///< Determine if vectors are equal.
	bool operator !=( const Vector2& v ) const; ///< Determine if vectors are not equal.
	Vector2 operator +() const;
	Vector2 operator -() const; ///< Negate the vector.
	Vector2 operator *( float s ) const; ///< Scalar-vector multiplication.
	void operator *=( float s ); ///< Scalar-vector multiplication.
	Vector2 operator /( float s ) const; ///< Scalar-vector division.
	void operator /=( float s ); ///< Scalar-vector division.
	Vector2 operator +( const Vector2& v ) const; ///< Add two vectors.
	void operator +=( const Vector2& v ); ///< Add two vectors.
	Vector2 operator -( const Vector2& v ) const; ///< Subtract two vectors.
	void operator -=( const Vector2& v ); ///< Subtract two vectors.
	Vector2 operator *( const Vector2& v ) const; ///< Multiply two vectors (actually scale the vector with another vector).
	void operator *=( const Vector2& v ); ///< Multiply two vectors (actually scale the vector with another vector).
	bool equals( const Vector2& v ) const; ///< Determine if vectors are equal.
	Vector2& negate(); ///< Negate the vector.
	Vector2 mul( const float s ) const; ///< Scalar-vector multiplication.
	Vector2 div( const float s ) const; ///< Scalar-vector division.
	Vector2 add( const Vector2& v ) const; ///< Add two vectors.
	Vector2 sub( const Vector2& v ) const; ///< Subtract two vectors.
	float dot( const Vector2& v ) const; ///< Vector dot-product.
	
	static const Vector2 Null;
	static const Vector2 XAxis;
	static const Vector2 YAxis;
	static const Vector2 UnitScale;

};

template <>
inline std::string toString<Vector2>( const Vector2& v )
{
	std::ostringstream oss;

	oss << v.x;
	oss << " ";
	oss << v.y;

	return oss.str();
}

template <>
inline Vector2 fromString<Vector2>( const std::string& tstr )
{
	Vector2 v;
	std::istringstream iss(tstr);

	if( iss.eof() )
		return v;
	iss >> v.x;

	if( iss.eof() )
		return v;
	iss >> v.y;

	return v;
}

}

#endif // __Vector2_h__
