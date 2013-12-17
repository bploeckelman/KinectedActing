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

#include "zhVector.h"
#include "zhMathMacros.h"
#include "zhMatrix.h"

namespace zh
{

Vector::Vector()
{
	mN = 0;
	mV = NULL;
}

Vector::Vector( unsigned int n, float x )
{
	zhAssert( n > 0 );

	mN = n;
	mV = new float[n];

	for( unsigned int i = 0; i < n; ++i )
		mV[i] = x;
}

Vector::Vector( const Vector& v )
{
	mN = v.size();
	mV = new float[mN];

	for( unsigned int i = 0; i < mN; ++i )
		this->set( i, v.get(i) );
}

Vector::~Vector()
{
	if( mV != NULL )
		delete[] mV;
}

unsigned int Vector::size() const
{
	return mN;
}

float Vector::get( unsigned int i ) const
{
	zhAssert( i < mN );

	return mV[i];
}

void Vector::set( unsigned int i, float x )
{
	zhAssert( i < mN );

	mV[i] = x;
}

Vector& Vector::null()
{
	memset( mV, 0, mN * sizeof(float) );

	return *this;
}

float Vector::lengthSq() const
{
	float len = 0.f;

	for( unsigned int i = 0; i < mN; ++i )
		len += mV[i] * mV[i];

	return len;
}

float Vector::length() const
{
	return sqrt( lengthSq() );
}

Vector& Vector::normalize()
{
	float inv_len = 1.f / length();

	for( unsigned int i = 0; i < mN; ++i )
		mV[i] *= inv_len;

	return *this;
}

Vector Vector::getNormalized() const
{
	Vector v = *this;
	return v.normalize();
}

float Vector::sum() const
{
	float s = 0.f;

	for( unsigned int i = 0; i < mN; ++i )
		s += mV[i];

	return s;
}

float Vector::distance( const Vector& v ) const
{
	return sqrt( distanceSq(v) );
}

float Vector::distanceSq( const Vector& v ) const
{
	zhAssert( mN == v.size() );

	float d = 0;
	float d1;

	for( unsigned int i = 0; i < mN; ++i )
	{
		d1 = get(i) - v.get(i);
		d += ( d1 * d1 );
	}

	return d;
}

float Vector::angle( const Vector& v ) const
{
	return acos( this->dot(v) );
}

float& Vector::operator[]( int i )
{
	zhAssert( (unsigned int)i < mN );

	return mV[i];
}

float Vector::operator[]( int i ) const
{
	zhAssert( (unsigned int)i < mN );

	return mV[i];
}

bool Vector::operator ==( const Vector& v ) const
{
	if( mN != v.size() )
		return false;

	for( unsigned int i = 0; i < mN; ++i )
		if( !zhEqualf( this->get(i), v.get(i) ) )
			return false;

	return true;
}

bool Vector::operator !=( const Vector& v ) const
{
	return !( *this == v );
}

Vector Vector::operator +() const
{
	return *this;
}

Vector Vector::operator -() const
{
	Vector v1(mN);

	for( unsigned int i = 0; i < mN; ++i )
		v1.set( i, -this->get(i) );

	return v1;
}

Vector Vector::operator *( float s ) const
{
	Vector v1(mN);

	for( unsigned int i = 0; i < mN; ++i )
		v1[i] = mV[i] * s;

	return v1;
}

void Vector::operator *=( float s )
{
	*this = *this * s;
}

Vector Vector::operator /( float s ) const
{
	Vector v1(mN);

	for( unsigned int i = 0; i < mN; ++i )
		v1[i] = mV[i] / s;

	return v1;
}

void Vector::operator /=( float s )
{
	*this = *this / s;
}

Vector Vector::operator +( const Vector& v ) const
{
	zhAssert( mN == v.size() );

	Vector v1(mN);

	for( unsigned int i = 0; i < mN; ++i )
		v1.set( i, this->get(i) + v.get(i) );

	return v1;
}

void Vector::operator +=( const Vector& v )
{
	*this = *this + v;
}

Vector Vector::operator -( const Vector& v ) const
{
	zhAssert( mN == v.size() );

	Vector v1(mN);

	for( unsigned int i = 0; i < mN; ++i )
		v1.set( i, this->get(i) - v.get(i) );

	return v1;
}

void Vector::operator -=( const Vector& v )
{
	*this = *this - v;
}

Vector Vector::operator *( const Vector& v ) const
{
	zhAssert( mN == v.size() );

	Vector v1(mN);

	for( unsigned int i = 0; i < mN; ++i )
		 v1.set( i, this->get(i) * v.get(i) );

	return v1;
}

Vector Vector::operator *( const Matrix& mat ) const
{
	zhAssert( mN == mat.size() );

	Vector res(mN);

	for( unsigned int i = 0; i < mN; ++i )
	{
		float rt = 0;

		for( unsigned int j = 0; j < mN; ++j )
			rt += ( mat.get(j,i) * get(j) );

		res.set( i, rt );
	}

	return res;
}

void Vector::operator *=( const Matrix& mat )
{
	*this = *this * mat;
}

Vector& Vector::operator =( const Vector& v )
{
	if( this == &v )
		return *this;

	if( mN != v.size() )
	{
		mN = v.size();
		if( mV != NULL )
			delete[] mV;
		mV = new float[mN];
	}

	for( unsigned int i = 0; i < mN; ++i )
		this->set( i, v.get(i) );

	return *this;
}

bool Vector::equals( const Vector& v ) const
{
	return *this == v;
}

Vector& Vector::negate()
{
	*this = -*this;

	return *this;
}

Vector Vector::mul( const float s ) const
{
	return *this * s;
}

Vector Vector::mul( const Vector& v ) const
{
	return *this * v;
}

Vector Vector::mul( const Matrix& mat ) const
{
	return *this * mat;
}

Vector Vector::div( const float s ) const
{
	return *this/s;
}

Vector Vector::add( const Vector& v ) const
{
	return *this + v;
}

Vector Vector::sub( const Vector& v ) const
{
	return *this - v;
}

float Vector::dot( const Vector& v ) const
{
	zhAssert( mN == v.size() );

	float d = 0.f;

	for( unsigned int i = 0; i < mN; ++i )
		d += ( this->get(i) * v.get(i) );

	return d;
}

}
