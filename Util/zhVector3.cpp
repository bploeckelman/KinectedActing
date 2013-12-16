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

#include "zhVector3.h"
#include "zhMatrix4.h"
#include "zhQuat.h"

namespace zh
{

const Vector3 Vector3::Null = Vector3();
const Vector3 Vector3::XAxis = Vector3( 1, 0, 0 );
const Vector3 Vector3::YAxis = Vector3( 0, 1, 0 );
const Vector3 Vector3::ZAxis = Vector3( 0, 0, 1 );
const Vector3 Vector3::UnitScale = Vector3( 1, 1, 1 );

Vector3::Vector3()
{
	x = y = z = 0;
	w = 1;
}

Vector3::Vector3( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
	w = 1;
}

unsigned int Vector3::size() const
{
	return 3;
}

float Vector3::get( unsigned int i ) const
{
	zhAssert( i < 3 );

	if( i == 0 )
		return x;
	else if( i == 1 )
		return y;
	else // if( i == 2 )
		return z;
}

void Vector3::set( unsigned int i, float x )
{
	zhAssert( i < 3 );

	if( i == 0 )
		this->x = x;
	else if( i == 1 )
		this->y = x;
	else // if( i == 2 )
		this->z = x;
}

Vector3& Vector3::null()
{
	x = y = z = 0;

	return *this;
}

float Vector3::lengthSq() const
{
	return x*x + y*y + z*z;
}


float Vector3::length() const
{
	return sqrt( lengthSq() );
}

Vector3& Vector3::normalize()
{
	*this *= ( 1.f / length() );

	return *this;
}

Vector3 Vector3::getNormalized() const
{
	Vector3 v = *this;
	return v.normalize();
}

float Vector3::distance( const Vector3& v ) const
{
	return sqrt( distanceSq(v) );
}

float Vector3::distanceSq( const Vector3& v ) const
{
	float d = 0;
	float d1 = x - v.x;
	d += ( d1 * d1 );
	d1 = y - v.y;
	d += ( d1 * d1 );
	d1 = z - v.z;
	d += ( d1 * d1 );

	return d;
}

float Vector3::angle( const Vector3& v ) const
{
	return acos( zhClamp(this->dot(v),-1,1) );
}

bool Vector3::operator ==( const Vector3& v ) const
{
	if( !zhEqualf( x, v.x ) ||
		!zhEqualf( y, v.y ) ||
		!zhEqualf( z, v.z ) )
		return false;

	return true;
}

bool Vector3::operator !=( const Vector3& v ) const
{
	return !( *this == v );
}

Vector3 Vector3::operator +() const
{
	return *this;
}

Vector3 Vector3::operator -() const
{
	return Vector3( -x, -y, -z );
}

Vector3 Vector3::operator *( float s ) const
{
	return Vector3( x * s, y * s, z * s );
}

void Vector3::operator *=( float s )
{
	x *= s;
	y *= s;
	z *= s;
}

Vector3 Vector3::operator /( float s ) const
{
	return Vector3( x/s, y/s, z/s );
}

void Vector3::operator /=( float s )
{
	x /= s;
	y /= s;
	z /= s;
}

Vector3 Vector3::operator +( const Vector3& v ) const
{
	return Vector3( x + v.x, y + v.y, z + v.z );
}

void Vector3::operator +=( const Vector3& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
}

Vector3 Vector3::operator -( const Vector3& v ) const
{
	return Vector3( x - v.x, y - v.y, z - v.z );
}

void Vector3::operator -=( const Vector3& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

Vector3 Vector3::operator *( const Vector3& v ) const
{
	return Vector3( x * v.x, y * v.y, z * v.z );
}

void Vector3::operator *=( const Vector3& v )
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

Vector3 Vector3::operator*( const Matrix4& mat ) const
{
	Vector3 res;

	res.x = mat.get(0,0) * x + mat.get(1,0) * y + mat.get(2,0) * z + mat.get(3,0);
	res.y = mat.get(0,1) * x + mat.get(1,1) * y + mat.get(2,1) * z + mat.get(3,1);
	res.z = mat.get(0,2) * x + mat.get(1,2) * y + mat.get(2,2) * z + mat.get(3,2);
	res.w = 1.f;

	return res;
}

void Vector3::operator*=( const Matrix4& mat )
{
	*this = *this * mat;
}

bool Vector3::equals( const Vector3& v ) const
{
	return *this == v;
}

Vector3& Vector3::negate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

Vector3 Vector3::mul( const float s ) const
{
	return *this * s;
}

Vector3 Vector3::div( const float s ) const
{
	return *this/s;
}

Vector3 Vector3::add( const Vector3& v ) const
{
	return *this + v;
}

Vector3 Vector3::sub( const Vector3& v ) const
{
	return *this - v;
}

float Vector3::dot( const Vector3& v ) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::cross( const Vector3& v ) const
{
	Vector3 res;
	res.x = y * v.z - z * v.y;
	res.y = z * v.x - x * v.z;
	res.z = x * v.y - y * v.x;

	return res;
}

Vector3 Vector3::mul( const Matrix4& mat ) const
{
	return *this * mat;
}

Vector3& Vector3::rotate( const Quat& rot )
{
	Quat vq = Quat( 0, x, y, z );
	vq = rot * vq;
	vq *= rot.getConjugate();
	x = vq.x;
	y = vq.y;
	z = vq.z;

	return *this;
}

Vector3 Vector3::getRotated( const Quat& rot ) const
{
	Vector3 result = *this;
	result.rotate(rot);
	
	return result;
}

Quat Vector3::getRotationTo( const Vector3& v ) const
{
	Quat rot;
	Vector3 v1 = *this;
	Vector3 v2 = v;

	float vd = v1.dot(v2);
	if( zhEqualf( vd, -1.f ) )
	{
		Vector3 axis = Vector3::XAxis.cross(*this);
		if( zhEqualf( axis.lengthSq(), 0 ) )
			axis = Vector3::YAxis.cross(*this);
		axis.normalize();
		rot = Quat( axis, zhPI );
	}
	else if( vd < 1.f )
	{
		float s = sqrt( ( 1.f + vd )*2.f ),
			inv_s = 1.f/s;

		Vector3 axis = v1.cross(v2);
		rot.x = axis.x * inv_s;
		rot.y = axis.y * inv_s;
		rot.z = axis.z * inv_s;
		rot.w = s * 0.5f;
		rot.normalize();
	}

	return rot;
}

}
