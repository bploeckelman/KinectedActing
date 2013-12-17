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

#ifndef __zhVector_h__
#define __zhVector_h__

#include "zhPrereq.h"
#include "zhString.h"

namespace zh
{

class Matrix;

/**
* @brief Vector class, representing an N-dimensional vector
* or point in N-dimensional space.
*/
class Vector
{

public:

	Vector(); ///< Constructor. Creates an uninitialized vector.
	explicit Vector( unsigned int n, float x = 0.f ); ///< Constructor. Creates a vector and initializes all elements with specific value.
	Vector( const Vector& v ); ///< Copy constructor.
	~Vector(); ///< Destructor.
	unsigned int size() const; ///< Gets the vector size.
	float get( unsigned int i ) const; ///< Gets an element of the vector by index.
	void set( unsigned int i, float x ); ///< Sets an element of the vector by index.
	Vector& null(); ///< Initializes this vector as a null-vector.
	float lengthSq() const; ///< Gets the vector length.
	float length() const; ///< Gets the vector length.
	Vector& normalize(); ///< Normalizes the current vector.
	Vector getNormalized() const; ///< Gets the current vector in normalized form.
	float sum() const; ///< Gets the sum of vector components.
	float distance( const Vector& v ) const; ///< Compute distance between vectors.
	float distanceSq( const Vector& v ) const; ///< Compute squared distance between vectors.
	float angle( const Vector& v ) const; ///< Computes the angle between vectors.
	float& operator[]( int i ); ///< Array subscript operator.
	float operator[]( int i ) const; ///< Array subscript operator.
	bool operator ==( const Vector& v ) const; ///< Returns true if vectors are equal.
	bool operator !=( const Vector& v ) const; ///< Returns true if vectors are not equal.
	Vector operator +() const;
	Vector operator -() const; ///< Negates the vector.
	Vector operator *( float s ) const; ///< Scalar-vector multiplication.
	void operator *=( float s ); ///< Scalar-vector multiplication.
	Vector operator /( float s ) const; ///< Scalar-vector division.
	void operator /=( float s ); ///< Scalar-vector division.
	Vector operator +( const Vector& v ) const; ///< Adds two vectors.
	void operator +=( const Vector& v ); ///< Adds two vectors.
	Vector operator -( const Vector& v ) const; ///< Subtracts two vectors.
	void operator -=( const Vector& v ); ///< Subtracts two vectors.
	Vector operator *( const Vector& v ) const; ///< Vector product.
	Vector operator *( const Matrix& mat ) const; ///< Multiply the vector with a matrix.
	void operator *=( const Matrix& mat ); ///< Multiply the vector with a matrix.
	Vector& operator =( const Vector& v ); ///< Vector assignment.
	bool equals( const Vector& v ) const; ///< Returns true if vectors are equal.
	Vector& negate(); ///< Negates the vector.
	Vector mul( const float s ) const; ///< Scalar-vector multiplication.
	Vector mul( const Vector& v ) const; ///< Vector-vector multiplication.
	Vector mul( const Matrix& mat ) const; ///< Multiply the current vector with a matrix.
	Vector div( const float s ) const; ///< Scalar-vector division.
	Vector add( const Vector& v ) const; ///< Adds two vectors.
	Vector sub( const Vector& v ) const; ///< Subtracts two vectors.
	float dot( const Vector& v ) const; ///< Vector dot-product.

	// TODO: overload sizeof here and in Matrix class?

private:

	unsigned int mN; ///< Vector dimensionality.
	float* mV; ///< Vector components.

};

template <>
inline std::string toString<Vector>( const Vector& v )
{
	std::ostringstream oss;

	for( unsigned int i = 0; i < v.size(); ++i )
	{
		oss << v[i];
		
		if( i < v.size() - 1 )
			oss << " ";
	}

	return oss.str();
}

template <>
inline Vector fromString<Vector>( const std::string& tstr )
{
	std::istringstream iss(tstr);

	unsigned int n = 0;
	float x;
	while( !iss.eof() )
	{
		iss >> x;
		++n;
	}

	Vector v(n);
	n = 0;
	iss.seekg(0);
	while( !iss.eof() )
	{
		iss >> v[n];
		++n;
	}

	return v;
}

}

#endif // __zhVector_h__
