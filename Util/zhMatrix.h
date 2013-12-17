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

#ifndef __zhMatrix_h__
#define __zhMatrix_h__

#include "zhPrereq.h"
#include "zhVector.h"

namespace zh
{

/**
* NxN square matrix class. Also contains a static method for solving
* linear equations.
*/
class Matrix
{

public:

	/**
	* Constructor. Creates an identity matrix.
	*/
	Matrix();

	/**
	* Constructor. Creates an identity matrix.
	*
	* @param Matrix size.
	*/
	explicit Matrix( unsigned int n );

	/**
	* Copy constructor.
	*/
	Matrix( const Matrix& mat );

	/**
	* Destructor.
	*/
	~Matrix();

	/**
	* Gets the matrix size.
	*/
	unsigned int size() const;

	/**
	* Gets an element of the matrix by index.
	*/
	float get( unsigned int i, unsigned int j ) const;

	/**
	* Sets an element of the matrix by index.
	*/
	void set( unsigned int i, unsigned int j, float x );

	/**
	* Sets the matrix to identity.
	*/
	Matrix& identity();

	/**
	* Transposes the matrix.
	*/
	Matrix& transpose();

	/**
	* Gets the transpose of the current matrix.
	*/
	Matrix getTranspose() const;

	/**
	* Inverts the matrix.
	*/
	Matrix& invert();

	/**
	* Gets the inverse of the current matrix.
	*/
	Matrix getInverse() const;

	/**
	* Solves the linear system given by the matrix.
	*/
	void solve( const Vector& b, Vector& x );

	/**
	* Performs LU decomposition of the matrix.
	*/
	Matrix& LUDecompose( std::vector<unsigned int>& perm );

	/**
	* Solves the linear system given by the matrix
	* using forward substitution.
	*/
	void forwardSubst( const Vector& b, const std::vector<unsigned int>& ordering, Vector& y ) const;

	/**
	* Solves the linear system given by the matrix
	* using back substitution.
	*/
	void backSubst( const Vector& y, Vector& x ) const;

	/**
	* Returns true if two matrices are equal.
	*/
	bool operator ==( const Matrix& mat ) const;

	/**
	* Returns true if two matrices are not equal.
	*/
	bool operator !=( const Matrix& mat ) const;

	/**
	* Multiplies two matrices.
	*/
	Matrix operator*( const Matrix& mat ) const;

	/**
	* Multiplies two matrices.
	*/
	void operator *=( const Matrix& mat );

	/**
	* Multiplies the matrix with a vector.
	*/
	Vector operator*( const Vector& v ) const;

	/**
	* Matrix assigment.
	*/
	Matrix& operator =( const Matrix& mat );

	/**
	* Returns true if two matrices are equal.
	*/
	bool equals( const Matrix& mat ) const;

	/**
	* Multiplies two matrices.
	*/
	Matrix mul( const Matrix& mat ) const;

	/**
	* Multiplies the current matrix with a vector.
	*/
	Vector mul( const Vector& v ) const;

private:

	unsigned int mN; ///< Matrix size.
	float* mMat; ///< Matrix elements.

};

}

#endif // __zhMatrix_h__
