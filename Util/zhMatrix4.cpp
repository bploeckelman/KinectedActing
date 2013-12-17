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

#include "zhMatrix4.h"
#include "zhQuat.h"

namespace zh
{

const Matrix4 Matrix4::Identity = Matrix4();

Matrix4::Matrix4()
{
	identity();
}

Matrix4::Matrix4( float ax, float ay, float az, EulerRotOrder order )
{
	euler( ax, ay, az, order );
}

Matrix4::Matrix4( const Vector3& axis, float angle )
{
	axisAngle( axis, angle );
}

Matrix4::Matrix4( const Quat& rot )
{
	quat(rot);
}

float Matrix4::get( int i, int j ) const
{
	#ifdef _DEBUG
	assert( i >= 0 || i <= 3 || j >= 0 || j <= 3 );
	#endif

	return m[i][j];
}

void Matrix4::set( int i, int j, float x )
{
	#ifdef _DEBUG
	assert( i >= 0 || i <= 3 || j >= 0 || j <= 3 );
	#endif

	m[i][j] = x;
}

Matrix4& Matrix4::identity()
{
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
	m[0][1] = m[0][2] = m[0][3]
		= m[1][0] = m[1][2] = m[1][3]
		= m[2][0] = m[2][1] = m[2][3]
		= m[3][0] = m[3][1] = m[3][2] = 0.f;

	return *this;
}

Matrix4& Matrix4::translation( float x, float y, float z )
{
	identity();

	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;

	return *this;
}

Matrix4& Matrix4::translation( const Vector3& trans )
{
	translation( trans.x, trans.y, trans.z );

	return *this;
}

Matrix4& Matrix4::euler( float ax, float ay, float az, EulerRotOrder order )
{
	identity();

	float A = cos(ax),
		B = sin(ax),
		C = cos(ay),
		D = sin(ay),
		E = cos(az),
		F = sin(az);

	if( order == EulerRotOrder_XYZ )
	{
		m[0][0] = C*E;
		m[0][1] = -C*F;
		m[0][2] = D;
		m[1][0] = B*D*E + A*F;
		m[1][1] = -B*D*F + A*E;
		m[1][2] = -B*C;
		m[2][0] = -A*D*E + B*F;
		m[2][1] = A*D*F + B*E;
		m[2][2] = A*C;
	}
	else if( order == EulerRotOrder_XZY )
	{
		m[0][0] = C*E;
		m[0][1] = -F;
		m[0][2] = D*E;
		m[1][0] = A*C*F + B*D;
		m[1][1] = A*E;
		m[1][2] = A*D*F - B*C;
		m[2][0] = B*C*F - A*D;
		m[2][1] = B*E;
		m[2][2] = B*D*F + A*C;
	}
	else if( order == EulerRotOrder_YXZ )
	{
		m[0][0] = C*E + B*D*F;
		m[0][1] = -C*F + B*D*E;
		m[0][2] = A*D;
		m[1][0] = A*F;
		m[1][1] = A*E;
		m[1][2] = -B;
		m[2][0] = -D*E + B*C*F;
		m[2][1] = D*F + B*C*E;
		m[2][2] = A*C;
	}
	else if( order == EulerRotOrder_YZX )
	{
		m[0][0] = C*E;
		m[0][1] = -A*C*F + B*D;
		m[0][2] = B*C*F + A*D;
		m[1][0] = F;
		m[1][1] = A*E;
		m[1][2] = -B*E;
		m[2][0] = -D*E;
		m[2][1] = A*D*F + B*C;
		m[2][2] = -B*D*F + A*C;
	}
	else if( order == EulerRotOrder_ZXY )
	{
		m[0][0] = C*E - B*D*F;
		m[0][1] = -A*F;
		m[0][2] = B*C*F + D*E;
		m[1][0] = C*F + B*D*E;
		m[1][1] = A*E;
		m[1][2] = D*F - B*C*E;
		m[2][0] = -A*D;
		m[2][1] = B;
		m[2][2] = A*C;
	}
	else if( order == EulerRotOrder_ZYX )
	{
		m[0][0] = C*E;
		m[0][1] = -A*F + B*D*E;
		m[0][2] = B*F + A*D*E;
		m[1][0] = C*F;
		m[1][1] = A*E + B*D*F;
		m[1][2] = A*D*F - B*E;
		m[2][0] = -D;
		m[2][1] = B*C;
		m[2][2] = A*C;
	}

	return *this;
}

Matrix4& Matrix4::axisAngle( const Vector3& axis, float angle )
{
	identity();

	float cosa = cos(angle),
		sina = sin(angle);

	m[0][0] = cosa + axis.x * axis.x * ( 1.f - cosa );
	m[1][0] = axis.z * sina + axis.y * axis.x * ( 1.f - cosa );
	m[2][0] = - axis.y * sina + axis.z * axis.x * ( 1.f - cosa );

	m[0][1] = - axis.z * sina + axis.x * axis.y * ( 1.f - cosa );
	m[1][1] = cosa + axis.y * axis.y * ( 1.f - cosa );
	m[2][1] = axis.x * sina + axis.z * axis.y * ( 1.f - cosa );
	
	m[0][2] = axis.y * sina + axis.x * axis.z * ( 1.f - cosa );
	m[1][2] = - axis.x * sina + axis.y * axis.z * ( 1.f - cosa );
	m[2][2] = cosa + axis.z * axis.z * ( 1.f - cosa );

	m[3][3] = 1.f;
	m[3][0] = m[3][1] = m[3][2] = m[0][3] = m[1][3] = m[2][3] = 0.f;

	return *this;
}

Matrix4& Matrix4::quat( const Quat& rot )
{
	identity();

	float xx = rot.x * rot.x,
		xy = rot.x * rot.y,
		xz = rot.x * rot.z,
		xw = rot.x * rot.w,
		yy = rot.y * rot.y,
		yz = rot.y * rot.z,
		yw = rot.y * rot.w,
		zz = rot.z * rot.z,
		zw = rot.z * rot.w;

	m[0][0] = 1.f - 2.f * ( yy + zz );
	m[0][1] = 2.f * ( xy - zw );
	m[0][2] = 2.f * ( xz + yw );

	m[1][0] = 2.f * ( xy + zw );
	m[1][1] = 1.f - 2.f * ( xx + zz );
	m[1][2] = 2.f * ( yz - xw );

	m[2][0] = 2.f * ( xz - yw );
	m[2][1] = 2.f * ( yz + xw );
	m[2][2] = 1.f - 2.f * ( xx + yy );

	m[3][3] = 1.f;
	m[3][0] = m[3][1] = m[3][2] = m[0][3] = m[1][3] = m[2][3] = 0.f;

	return *this;
}

Matrix4& Matrix4::scale( float sx, float sy, float sz )
{
	identity();

	m[0][0] = sx;
	m[1][1] = sy;
	m[2][2] = sz;

	return *this;
}

Matrix4& Matrix4::scale( const Vector3& scal )
{
	scale( scal.x, scal.y, scal.z );

	return *this;
}

Matrix4& Matrix4::transpose()
{
	float t;

	t = m[0][1];
	m[0][1] = m[1][0];
	m[1][0] = t;

	t = m[0][2];
	m[0][2] = m[2][0];
	m[2][0] = t;

	t = m[0][3];
	m[0][3] = m[3][0];
	m[3][0] = t;

	t = m[1][2];
	m[1][2] = m[2][1];
	m[2][1] = t;

	t = m[1][3];
	m[1][3] = m[3][1];
	m[3][1] = t;

	t = m[2][3];
	m[2][3] = m[3][2];
	m[3][2] = t;

	return *this;
}

Matrix4 Matrix4::getTranspose() const
{
	Matrix4 mat = *this;
	mat.transpose();
	return mat;
}

float Matrix4::trace() const
{
	return m[0][0] + m[1][1] + m[2][2] + m[3][3];
}

float Matrix4::determinant() const
{
	float det3, det = 0.f;
	float sub_mat[3][3];
	
	float sgn;
	int j;
	for( sgn = 1.f, j = 0; j < 4; ++j, sgn *= -1.f )
	{
		getSubMatrix3x3( 0, j, &sub_mat[0][0] );
		det3 = determinant3( &sub_mat[0][0] );
		det += ( m[0][j] * det3 * sgn );
	}

	return det;
}

Matrix4& Matrix4::invert()
{
	if( *this == Matrix4() ) //if matrix is identity
		return *this;

	if( getScale() == Vector3( 1.f, 1.f, 1.f )
		&& getTranslation() == Vector3( 0.f, 0.f, 0.f ) ) //if matrix is rotation matrix
		return this->transpose();

	//compute inverse
	
	Matrix4 t_mat = *this;
	float det = determinant();
	float sub_mat[3][3];
	int sgn;

	if( zhEqualf( det, 0.f ) )
		return this->identity();

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			sgn = 1 - ( ( i + j ) % 2 ) * 2;
			getSubMatrix3x3( i, j, &sub_mat[0][0] );
			t_mat.set( j, i, static_cast<float>(sgn) * determinant3( &sub_mat[0][0] ) / det );
		}
	}

	return ( *this = t_mat );
}

Matrix4 Matrix4::getInverse() const
{
	Matrix4 mat = *this;
	mat.invert();
	return mat;
}

bool Matrix4::operator ==( const Matrix4& mat ) const
{
	if( !zhEqualf( m[0][0], mat.m[0][0] ) ||
		!zhEqualf( m[0][1], mat.m[0][1] ) ||
		!zhEqualf( m[0][2], mat.m[0][2] ) ||
		!zhEqualf( m[0][3], mat.m[0][3] ) ||
		!zhEqualf( m[1][0], mat.m[1][0] ) ||
		!zhEqualf( m[1][1], mat.m[1][1] ) ||
		!zhEqualf( m[1][2], mat.m[1][2] ) ||
		!zhEqualf( m[1][3], mat.m[1][3] ) ||
		!zhEqualf( m[2][0], mat.m[2][0] ) ||
		!zhEqualf( m[2][1], mat.m[2][1] ) ||
		!zhEqualf( m[2][2], mat.m[2][2] ) ||
		!zhEqualf( m[2][3], mat.m[2][3] ) ||
		!zhEqualf( m[3][0], mat.m[3][0] ) ||
		!zhEqualf( m[3][1], mat.m[3][1] ) ||
		!zhEqualf( m[3][2], mat.m[3][2] ) ||
		!zhEqualf( m[3][3], mat.m[3][3] ) )
		return false;

	return true;
}

bool Matrix4::operator !=( const Matrix4& mat ) const
{
	return !( *this == mat );
}

Matrix4 Matrix4::operator*( const Matrix4& mat ) const
{
	Matrix4 res;

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			res.m[i][j] = 0;

			for( int k = 0; k < 4; ++k )
			{
				res.m[i][j] += m[i][k] * mat.m[k][j];
			}
		}
	}

	return res;
}

void Matrix4::operator *=( const Matrix4& mat )
{
	*this = *this * mat;
}

Vector3 Matrix4::operator*( const Vector3& v ) const
{
	Vector3 res;

	res.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
	res.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
	res.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];

	return res;
}

bool Matrix4::equals( const Matrix4& mat ) const
{
	return *this == mat;
}

Matrix4 Matrix4::mul( const Matrix4& mat ) const
{
	return *this * mat;
}

Vector3 Matrix4::mul( const Vector3& v ) const
{
	return *this * v;
}

void Matrix4::getEuler( float& ax, float& ay, float& az, EulerRotOrder order ) const
{
	Matrix4 mat = getRotationMatrix();
	float A, C, E;

	if( order == EulerRotOrder_XYZ )
	{
		ay = asin( mat.m[0][2] );
		C = cos(ay);

		if( !zhEqualf_t( C, 0, 0.05 ) )
		{
			ax = atan2( -mat.m[1][2]/C, mat.m[2][2]/C );
			az = atan2( -mat.m[0][1]/C, mat.m[0][0]/C );
		}
		else
		{
			ax = 0;
			az = atan2( mat.m[1][0], mat.m[1][1] );
		}
	}
	else if( order == EulerRotOrder_XZY )
	{
		az = asin( -mat.m[0][1] );
		E = cos(az);

		if( !zhEqualf_t( E, 0, 0.05 ) )
		{
			ax = atan2( mat.m[2][1]/E, mat.m[1][1]/E );
			ay = atan2( mat.m[0][2]/E, mat.m[0][0]/E );
		}
		else
		{
			ax = 0;
			ay = atan2( mat.m[1][2], mat.m[1][0] );
		}
	}
	else if( order == EulerRotOrder_YXZ )
	{
		ax = asin( -mat.m[1][2] );
		A = cos(ax);

		if( !zhEqualf_t( A, 0, 0.05 ) )
		{
			ay = atan2( mat.m[0][2]/A, mat.m[2][2]/A );
			az = atan2( mat.m[1][0]/A, mat.m[1][1]/A );
		}
		else
		{
			ay = 0;
			az = atan2( -mat.m[0][1], mat.m[0][0] );
		}
	}
	else if( order == EulerRotOrder_YZX )
	{
		az = asin( mat.m[1][0] );
		E = cos(az);

		if( !zhEqualf_t( E, 0, 0.05 ) )
		{
			ax = atan2( -mat.m[1][2]/E, mat.m[1][1]/E );
			ay = atan2( -mat.m[2][0]/E, mat.m[0][0]/E );
		}
		else
		{
			ay = 0;
			ax = atan2( mat.m[2][1], mat.m[2][2] );
		}
	}
	else if( order == EulerRotOrder_ZXY )
	{
		ax = asin( mat.m[2][1] );
		A = cos(ax);

		if( !zhEqualf_t( A, 0, 0.05 ) )
		{
			ay = atan2( -mat.m[2][0]/A, mat.m[2][2]/A );
			az = atan2( -mat.m[0][1]/A, mat.m[1][1]/A );
		}
		else
		{
			az = 0;
			ay = atan2( mat.m[0][2], mat.m[0][0] );
		}
	}
	else if( order == EulerRotOrder_ZYX )
	{
		ay = asin( -mat.m[2][0] );
		C = cos(ay);

		if( !zhEqualf_t( C, 0, 0.05 ) )
		{
			ax = atan2( mat.m[2][1]/C, mat.m[2][2]/C );
			az = atan2( mat.m[1][0]/C, mat.m[0][0]/C );
		}
		else
		{
			az = 0;
			ax = atan2( -mat.m[1][2], mat.m[1][1] );
		}
	}
}

Matrix4& Matrix4::setEuler( float ax, float ay, float az, EulerRotOrder order )
{
	Matrix4 mat( ax, ay, az, order );
	setRotationMatrix( mat );

	return *this;
}

void Matrix4::getAxisAngle( Vector3& axis, float& angle ) const
{
	Quat q = getQuat();
	q.getAxisAngle( axis, angle );
}

Matrix4& Matrix4::setAxisAngle( const Vector3& axis, float angle )
{
	Matrix4 mat( axis, angle );
	setRotationMatrix( mat );

	return *this;
}

Quat Matrix4::getQuat() const
{
	Quat q;
	Matrix4 mat = getRotationMatrix();
	float tr = mat.trace(), s;

	// if trace > 0, perform instant computation
	if( tr > 0.f )
	{
		s = sqrt(tr) * 2.f;
		q.x = ( mat.m[2][1] - mat.m[1][2] ) / s;
		q.y = ( mat.m[0][2] - mat.m[2][0] ) / s;
		q.z = ( mat.m[1][0] - mat.m[0][1] ) / s;
		q.w = 0.25f * s;
	}

	// find which column holds the largest major diagonal element
	float max_elem = -FLT_MAX;
	int max_col = -1;
	for( int i = 0; i < 3; ++i )
	{
		if( mat.m[i][i] > max_elem )
		{
			max_elem = mat.m[i][i];
			max_col = i;
		}
	}

	// compute quaternion
	switch(max_col)
	{

	case 0:

		s  = sqrt( 1.f + mat.m[0][0] - mat.m[1][1] - mat.m[2][2] ) * 2.f;
		q.x = 0.25f * s;
		q.y = ( mat.m[1][0] + mat.m[0][1] ) / s;
		q.z = ( mat.m[2][0] + mat.m[0][2] ) / s;
		q.w = ( mat.m[2][1] - mat.m[1][2] ) / s;

		break;

	case 1:

		s  = sqrt( 1.f - mat.m[0][0] + mat.m[1][1] - mat.m[2][2] ) * 2.f;
		q.x = ( mat.m[1][0] + mat.m[0][1] ) / s;
		q.y = 0.25f * s;
		q.z = ( mat.m[2][1] + mat.m[1][2] ) / s;
		q.w = ( - mat.m[2][0] + mat.m[0][2] ) / s;

		break;

	case 2:

		s  = sqrt( 1.f - mat.m[0][0] - mat.m[1][1] + mat.m[2][2] ) * 2.f;
		q.x = ( mat.m[2][0] + mat.m[0][2] ) / s;
		q.y = ( mat.m[2][1] + mat.m[1][2] ) / s;
		q.z = 0.25f * s;
		q.w = ( mat.m[1][0] - mat.m[0][1] ) / s;

		break;

	default:

		break;
	}

	return q;
}

Matrix4& Matrix4::setQuat( const Quat& rot )
{
	Matrix4 mat( rot );
	setRotationMatrix( mat );

	return *this;
}

Matrix4 Matrix4::getRotationMatrix() const
{
	Matrix4 mat;

	mat.m[0][0] = m[0][0];
	mat.m[0][1] = m[0][1];
	mat.m[0][2] = m[0][2];

	mat.m[1][0] = m[1][0];
	mat.m[1][1] = m[1][1];
	mat.m[1][2] = m[1][2];

	mat.m[2][0] = m[2][0];
	mat.m[2][1] = m[2][1];
	mat.m[2][2] = m[2][2];

	Vector3 scal = getScale();
	mat *= Matrix4().scale( 1.f / scal.x, 1.f / scal.y, 1.f / scal.z ); // remove scale

	return mat;
}

void Matrix4::setRotationMatrix( const Matrix4& mat )
{
	Vector3 scal = getScale();
	
	m[0][0] = mat.m[0][0];
	m[0][1] = mat.m[0][1];
	m[0][2] = mat.m[0][2];

	m[1][0] = mat.m[1][0];
	m[1][1] = mat.m[1][1];
	m[1][2] = mat.m[1][2];

	m[2][0] = mat.m[2][0];
	m[2][1] = mat.m[2][1];
	m[2][2] = mat.m[2][2];

	*this *= Matrix4().scale( scal );
}

void Matrix4::getTranslation( float& x, float& y, float& z ) const
{
	x = m[3][0];
	y = m[3][1];
	z = m[3][2];
}

Matrix4& Matrix4::setTranslation( float x, float y, float z )
{
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;

	return *this;
}

Vector3 Matrix4::getTranslation() const
{
	return Vector3( m[3][0], m[3][1], m[3][2] );
}

Matrix4& Matrix4::setTranslation( const Vector3& trans )
{
	m[3][0] = trans.x;
	m[3][1] = trans.y;
	m[3][2] = trans.z;

	return *this;
}

void Matrix4::getScale( float& sx, float& sy, float& sz ) const
{
	sx = sqrt( m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0] );
	sy = sqrt( m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1] );
	sz = sqrt( m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2] );
}

Matrix4& Matrix4::setScale( float sx, float sy, float sz )
{
	float sx0, sy0, sz0;

	getScale( sx0, sy0, sz0 );

	*this *= Matrix4().scale( sx / sx0, sy / sy0, sz / sz0 );

	return *this;
}

Vector3 Matrix4::getScale() const
{
	Vector3 v;
	getScale( v.x, v.y, v.z );
	return v;
}

Matrix4& Matrix4::setScale( const Vector3& scal )
{
	return setScale( scal.x, scal.y, scal.z );
}

float Matrix4::determinant3( const float* mat ) const
{
	return mat[0] * ( mat[4] * mat[8] - mat[5] * mat[7] )
		- mat[1] * ( mat[3] * mat[8] - mat[5] * mat[6] )
		+ mat[2] * ( mat[3] * mat[7] - mat[4] * mat[6] );
}

void Matrix4::getSubMatrix3x3( int i, int j, float* subMat ) const
{
	int di, dj, si, sj;

	// loop through 3x3 submatrix
	for( di = 0; di < 3; di ++ )
	{
		for( dj = 0; dj < 3; dj ++ )
		{
		  // map 3x3 element (destination) to 4x4 element (source)
		  si = di + ( ( di >= i ) ? 1 : 0 );
		  sj = dj + ( ( dj >= j ) ? 1 : 0 );
		  // copy element
		  subMat[di * 3 + dj] = m[si][sj];
		}
	}
}

}
