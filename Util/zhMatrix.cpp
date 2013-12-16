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

#include "zhMatrix.h"
#include "zhMathMacros.h"

namespace zh
{

Matrix::Matrix()
{
	mN = 0;
	mMat = NULL;

	identity();
}

Matrix::Matrix( unsigned int n )
{
	zhAssert( n > 0 );

	mN = n;
	mMat = new float[mN*mN];
	
	identity();
}

Matrix::Matrix( const Matrix& mat )
{
	mN = mat.size();
	mMat = new float[mN*mN];

	for( unsigned int i = 0; i < mN; ++i )
		for( unsigned int j = 0; j < mN; ++j )
			this->set(i,j,mat.get(i,j));
}

Matrix::~Matrix()
{
	if( mMat != NULL )
		delete[] mMat;
}

unsigned int Matrix::size() const
{
	return mN;
}

float Matrix::get( unsigned int i, unsigned int j ) const
{
	zhAssert( i < mN && j < mN );

	return mMat[i*mN+j];
}

void Matrix::set( unsigned int i, unsigned int j, float x )
{
	zhAssert( i < mN && j < mN );

	mMat[i*mN+j] = x;
}

Matrix& Matrix::identity()
{
	for( unsigned int i = 0; i < mN; ++i )
	{
		for( unsigned int j = 0; j < mN; ++j )
		{
			if( i == j )
				set( i, j, 1 );
			else
				set( i, j, 0 );
		}
	}

	return *this;
}

Matrix& Matrix::transpose()
{
	float t;

	for( unsigned int i = 0; i < mN; ++i )
	{
		for( unsigned int j = i; j < mN; ++j )
		{
			t = get( i, j );
			set( i, j, get( j, i ) );
			set( j, i, t );
		}
	}

	return *this;
}

Matrix Matrix::getTranspose() const
{
	Matrix mat = *this;
	mat.transpose();
	return mat;
}

Matrix& Matrix::invert()
{
	Matrix A = *this;
	std::vector<unsigned int> perm;
	A.LUDecompose(perm);
	
	Vector x(mN), e(mN), y(mN);

	for( unsigned int j = 0; j < mN; ++j )
	{
		e.null();
		e.set( j, 1.f );

		A.forwardSubst( e, perm, y );
		A.backSubst( y, x );

		for( unsigned int i = 0; i < mN; ++i )
			set( i, j, x.get(i) );
	}

	return *this;
}

Matrix Matrix::getInverse() const
{
	Matrix mat = *this;
	mat.invert();
	return mat;
}

void Matrix::solve( const Vector& b, Vector& x )
{
	zhAssert( size() == b.size() );

	std::vector<unsigned int> perm( size() );
	for( unsigned int i = 0; i < perm.size(); ++i )
		perm[i] = i;

	LUDecompose(perm);

	Vector y( size() );
	forwardSubst( b, perm, y );
	backSubst( y, x );
}

Matrix& Matrix::LUDecompose( std::vector<unsigned int>& perm )
{
	float pivot;
	unsigned int l = 0;

	perm.resize( size() );
	for( unsigned int i = 0; i < mN; ++i ) perm[i]= i;

	for( unsigned int k = 0; k < mN-1; ++k )
	{
		pivot = 0;
		
		for( unsigned int i = k; i < mN; ++i )
		{
			if( fabs( get(i,k) ) > pivot )
			{
				pivot = fabs( get(i,k) );
				l = i;
			}
		}

		if( zhEqualf( pivot, 0 ) ) // singular matrix
		{
			break;
		}

		unsigned int ti = perm[k];
		perm[k] = perm[l];
		perm[l] = ti;

		float tf;
		for( unsigned int j = 0; j < mN; ++j )
		{
			tf = get(k,j);
			set( k, j, get(l,j) );
			set( l, j, tf );
		}

		for( unsigned int i = k+1; i < mN; ++i )
		{
			set( i, k, get(i,k) / get(k,k) );

			for( unsigned int j = k+1; j < mN; ++j )
			{
				set( i, j, get(i,j) - get(i,k) * get(k,j) );
			}
		}
	}

	return *this;
}

void Matrix::forwardSubst( const Vector& b, const std::vector<unsigned int>& perm, Vector& y ) const
{
	zhAssert( size() == b.size() );
	zhAssert( size() == perm.size() );

	float rsum = 0;

	for( unsigned int i = 0; i < y.size(); ++i )
	{
		for( unsigned int j = 0; j < i; ++j )
		{
			rsum += get(i,j) * y.get(j);
		}

		y.set( i, b.get( perm[i] ) - rsum );
		rsum = 0;
	}
}

void Matrix::backSubst( const Vector& y, Vector& x ) const
{
	zhAssert( size() == y.size() );

	float rsum = 0;

	for( int i = (int)y.size()-1; i >= 0; --i )
	{
		for( int j = i+1; j < (int)y.size(); ++j )
		{
			rsum += get(i,j) * x.get(j);
		}

		x.set( i, ( y.get(i) - rsum ) / get(i,i) );
		rsum = 0;
	}
}

bool Matrix::operator ==( const Matrix& mat ) const
{
	if( mN != mat.size() )
		return false;

	for( unsigned int i = 0; i < size(); ++i )
	{
		for( unsigned int j = 0; j < size(); ++j )
		{
			if( !zhEqualf( get( i, j ), mat.get( i, j ) ) )
				return false;
		}
	}

	return true;
}

bool Matrix::operator !=( const Matrix& mat ) const
{
	return !( *this == mat );
}

Matrix Matrix::operator*( const Matrix& mat ) const
{
	zhAssert( mN == mat.size() );

	Matrix res(mN);

	for( unsigned int i = 0; i < mN; ++i )
	{
		for( unsigned int j = 0; j < mN; ++j )
		{
			res.set(i,j,0);

			for( unsigned int k = 0; k < mN; ++k )
			{
				res.set( i, j, res.get(i,j) + get(i,k) * mat.get(k,j) );
			}
		}
	}

	return res;
}

void Matrix::operator *=( const Matrix& mat )
{
	*this = *this * mat;
}

Vector Matrix::operator*( const Vector& v ) const
{
	zhAssert( mN == v.size() );

	Vector res(mN);

	for( unsigned int i = 0; i < mN; ++i )
	{
		res.set( i, 0 );

		for( unsigned int j = 0; j < mN; ++j )
		{
			res.set( i, res.get(i) + v.get(i) * get( i, j ) );
		}
	}

	return res;
}

Matrix& Matrix::operator =( const Matrix& mat )
{
	if( this == &mat )
		return *this;

	if( mN != mat.size() )
	{
		mN = mat.size();
		if( mMat != NULL )
			delete[] mMat;
		mMat = new float[mN*mN];
	}

	for( unsigned int i = 0; i < mN; ++i )
		for( unsigned int j = 0; j < mN; ++j )
			this->set(i,j,mat.get(i,j));

	return *this;
}

bool Matrix::equals( const Matrix& mat ) const
{
	return *this == mat;
}

Matrix Matrix::mul( const Matrix& mat ) const
{
	return *this * mat;
}

Vector Matrix::mul( const Vector& v ) const
{
	return *this * v;
}

}
