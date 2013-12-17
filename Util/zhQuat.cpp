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

#include "zhQuat.h"
#include "zhVector3.h"
#include "zhMatrix4.h"

namespace zh
{

const Quat Quat::Identity = Quat();

Quat::Quat()
{
	w = 1.f;
	x = y = z = 0.f;
}

Quat::Quat( float w, float x, float y, float z )
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

Quat::Quat( float ax, float ay, float az, EulerRotOrder order )
{
	Vector3 x_axis = Vector3::XAxis,
		y_axis = Vector3::YAxis,
		z_axis = Vector3::ZAxis;
	Quat qx, qy, qz;
	qx = Quat( x_axis, ax );
	qy = Quat( y_axis, ay );
	qz = Quat( z_axis, az );

	if( order == EulerRotOrder_XYZ )
		*this = qx * qy * qz;
	else if( order == EulerRotOrder_XZY )
		*this = qx * qz * qy;
	else if( order == EulerRotOrder_YXZ )
		*this = qy * qx * qz;
	else if( order == EulerRotOrder_YZX )
		*this = qy * qz * qx;
	else if( order == EulerRotOrder_ZXY )
		*this = qz * qx * qy;
	else if( order == EulerRotOrder_ZYX )
		*this = qz * qy * qx;
}

Quat::Quat( const Vector3& axis, float angle )
{
	Vector3 t_axis;
	float cos_a, sin_a;
	
	t_axis = axis;
	t_axis.normalize();
	sin_a = sin( angle / 2.f );
	cos_a = cos( angle / 2.f );
	w = cos_a;
	x = axis.x * sin_a;
	y = axis.y * sin_a;
	z = axis.z * sin_a;
}

Quat& Quat::conjugate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

Quat Quat::getConjugate() const
{
	return Quat( w, -x, -y, -z );
}

float Quat::lengthSq() const
{
	return w * w + x * x + y * y + z * z;
}

float Quat::length() const
{
	float t = lengthSq();
	return zhEqualf( t, 1.f ) ? 1.f : sqrt(t);
}

Quat& Quat::normalize()
{
	float qmag = this->length();
	w /= qmag;
	x /= qmag;
	y /= qmag;
	z /= qmag;

	return *this;
}

Quat Quat::getNormalized() const
{
	Quat q = *this;
	return q.normalize();
}

Quat& Quat::invert()
{
	float qmag = w*w + x*x + y*y + z*z;
	float inv_qmag = 1.f / qmag;
	w *= inv_qmag;
	x *= -inv_qmag;
	y *= -inv_qmag;
	z *= -inv_qmag;

	return *this;
}

Quat Quat::getInverse() const
{
	Quat q = *this;
	float qmag = w*w + x*x + y*y + z*z;
	float inv_qmag = 1.f / qmag;
	q.w *= inv_qmag;
	q.x *= -inv_qmag;
	q.y *= -inv_qmag;
	q.z *= -inv_qmag;

	return q;
}

Quat Quat::exp() const
{
	Quat q = *this;
	float a = sqrt( x*x + y*y + z*z);
	float sin_a = sin(a);

	if( fabs(sin_a) >= 0.005f )
	{
		float c = sin_a / a;
		q.x *= c;
		q.y *= c;
		q.z *= c;
	}

	q.w = cos(a);

	return q;
}

Quat Quat::log() const
{
	Quat q= *this;

	if( fabs(w) < 1.f )
	{
		float a = acos(w);
		float sin_a = sin(a);

		if( fabs(sin_a) >= 0.005f )
		{
			float c = a/sin_a;
			q.x *= c;
			q.y *= c;
			q.z *= c;
		}
	}

	q.w = 0;

	return q;
}

void Quat::getEuler( float& ax, float& ay, float& az, EulerRotOrder order ) const
{
	Matrix4(*this).getEuler( ax, ay, az, order );
}

float Quat::getAngle() const
{
	return acos(w) * 2.f;
}

void Quat::getAxisAngle( Vector3& axis, float& angle ) const
{
	float cos_a, sin_a;
	Quat q = *this;
	
	q.normalize();
	cos_a = q.w;
	angle = acos(cos_a) * 2.f;
	sin_a = sqrt( 1.f - cos_a * cos_a );
	if( zhEqualf( sin_a, 0.f ) ) sin_a = 1.f;
	axis.x = x / sin_a;
	axis.y = y / sin_a;
	axis.z = z / sin_a;
}

bool Quat::operator ==( const Quat& q ) const
{
	if( !zhEqualf( x, q.x ) ||
		!zhEqualf( y, q.y ) ||
		!zhEqualf( z, q.z ) || 
		!zhEqualf( w, q.w ) )
		return false;

	return true;
}

bool Quat::operator !=( const Quat& q ) const
{
	return !( *this == q );
}

Quat Quat::operator +() const
{
	return *this;
}

Quat Quat::operator -() const
{
	return Quat( -w, -x, -y, -z );
}

Quat Quat::operator +( const Quat& q ) const
{
	return Quat( this->w + q.w, this->x + q.x, this->y + q.y, this->z + q.z );
}

void Quat::operator +=( const Quat& q )
{
	*this = *this + q;
}

Quat Quat::operator -( const Quat& q ) const
{
	return Quat( this->w - q.w, this->x - q.x, this->y - q.y, this->z - q.z );
}

void Quat::operator -=( const Quat& q )
{
	*this = *this - q;
}

Quat Quat::operator *( float s ) const
{
	return Quat( s * w, s * x, s * y, s * z );
}

void Quat::operator *=( float s )
{
	*this = *this * s;
}

Quat Quat::operator /( float s ) const
{
	return Quat( w/s, x/s, y/s, z/s );
}

void Quat::operator /=( float s )
{
	*this = *this / s;
}

Quat Quat::operator *( const Quat& q ) const
{
	return Quat( w * q.w - x * q.x - y * q.y - z * q.z,
		w * q.x + x * q.w + y * q.z - z * q.y,
		w * q.y + y * q.w - x * q.z + z * q.x,
		w * q.z + z * q.w + x * q.y - y * q.x );
}

void Quat::operator *=( const Quat& q )
{
	*this = *this * q;
}

bool Quat::equals( const Quat& q ) const
{
	return *this == q;
}

Quat& Quat::negate()
{
	w = -w;
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

Quat Quat::add( const Quat& q ) const
{
	return *this + q;
}

float Quat::dot( const Quat& q ) const
{
	return w * q.w + x * q.x + y * q.y + z * q.z;
}

Quat Quat::cross( const Quat& q ) const
{
	return *this * q;
}

Quat Quat::slerp( const Quat& q, float t ) const
{
	float cos_a = this->dot(q);
	Quat q1;

	if( cos_a < 0.f )
	{
		cos_a = -cos_a;
		q1 = -q;
	}
	else
	{
		q1 = q;
	}

	if( fabs(cos_a) < 0.995f )
	{
		float sin_a = sqrt( 1.f - cos_a * cos_a );
		float a = atan2( sin_a, cos_a );
		float inv_sin_a = 1.f / sin_a;
		float qc1 = sin( ( 1.f - t ) * a ) * inv_sin_a;
		float qc2 = sin( t * a ) * inv_sin_a;
		q1 = *this * qc1 + q1 * qc2;
	}
	else
	{
		q1 = *this * ( 1.f - t ) + q1 * t;
		q1.normalize();
	}

	return q1;
}

Quat Quat::nlerp( const Quat& q, float t ) const
{
	Quat q1;
	float qa = this->dot(q);

	if( qa < 0.f )
		q1 = *this + ( -q - *this ) * t;
	else
		q1 = *this + ( q - *this ) * t;

	return q1.normalize();
}

Quat Quat::squad( const Quat& qa, const Quat& qb, const Quat& q, float t ) const
{
	float t0 = 2.f * t * ( 1.f - t );
	Quat qi1 = this->slerp( q, t);
	Quat qi2 = qa.slerp( qb, t );

	return qi1.slerp( qi2, t0 );
}

}
