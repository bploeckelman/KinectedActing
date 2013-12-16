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

#include "zhVector2.h"

namespace zh
{

const Vector2 Vector2::Null = Vector2();
const Vector2 Vector2::XAxis = Vector2( 1, 0 );
const Vector2 Vector2::YAxis = Vector2( 0, 1 );
const Vector2 Vector2::UnitScale = Vector2( 1, 1 );

Vector2::Vector2()
{
	x = y = 0;
}

Vector2::Vector2( float x, float y )
{
	this->x = x;
	this->y = y;
}

unsigned int Vector2::size() const
{
	return 2;
}

float Vector2::get( unsigned int i ) const
{
	zhAssert( i < 2 );

	if( i == 0 )
		return x;
	else // if( i == 1 )
		return y;
}

void Vector2::set( unsigned int i, float x )
{
	zhAssert( i < 2 );

	if( i == 0 )
		this->x = x;
	else // if( i == 1 )
		this->y = x;
}

Vector2& Vector2::null()
{
	x = y = 0;

	return *this;
}

float Vector2::lengthSq() const
{
	return x*x + y*y;
}

float Vector2::length() const
{
	return sqrt( lengthSq() );
}

Vector2& Vector2::normalize()
{
	*this *= ( 1.f / length() );

	return *this;
}

Vector2 Vector2::getNormalized() const
{
	Vector2 v = *this;
	return v.normalize();
}

float Vector2::distance( const Vector2& v ) const
{
	return sqrt( distanceSq(v) );
}

float Vector2::distanceSq( const Vector2& v ) const
{
	float d = 0;
	float d1 = x - v.x;
	d += ( d1 * d1 );
	d1 = y - v.y;
	d += ( d1 * d1 );

	return d;
}

float Vector2::angle( const Vector2& v ) const
{
	return acos( this->dot(v) );
}

bool Vector2::operator ==( const Vector2& v ) const
{
	if( !zhEqualf( x, v.x ) ||
		!zhEqualf( y, v.y ) )
		return false;

	return true;
}

bool Vector2::operator !=( const Vector2& v ) const
{
	return !( *this == v );
}

Vector2 Vector2::operator +() const
{
	return *this;
}

Vector2 Vector2::operator -() const
{
	return Vector2( -x, -y );
}

Vector2 Vector2::operator *( float s ) const
{
	return Vector2( x * s, y * s );
}

void Vector2::operator *=( float s )
{
	x *= s;
	y *= s;
}

Vector2 Vector2::operator /( float s ) const
{
	return Vector2( x/s, y/s );
}

void Vector2::operator /=( float s )
{
	x /= s;
	y /= s;
}

Vector2 Vector2::operator +( const Vector2& v ) const
{
	return Vector2( x + v.x, y + v.y );
}

void Vector2::operator +=( const Vector2& v )
{
	x += v.x;
	y += v.y;
}

Vector2 Vector2::operator -( const Vector2& v ) const
{
	return Vector2( x - v.x, y - v.y );
}

void Vector2::operator -=( const Vector2& v )
{
	x -= v.x;
	y -= v.y;
}

Vector2 Vector2::operator *( const Vector2& v ) const
{
	return Vector2( x * v.x, y * v.y );
}

void Vector2::operator *=( const Vector2& v )
{
	x *= v.x;
	y *= v.y;
}

bool Vector2::equals( const Vector2& v ) const
{
	return *this == v;
}

Vector2& Vector2::negate()
{
	x = -x;
	y = -y;

	return *this;
}

Vector2 Vector2::mul( const float s ) const
{
	return *this * s;
}

Vector2 Vector2::div( const float s ) const
{
	return *this/s;
}

Vector2 Vector2::add( const Vector2& v ) const
{
	return *this + v;
}

Vector2 Vector2::sub( const Vector2& v ) const
{
	return *this - v;
}

float Vector2::dot( const Vector2& v ) const
{
	return x * v.x + y * v.y;
}

}
