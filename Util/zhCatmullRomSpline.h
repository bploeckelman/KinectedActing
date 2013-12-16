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

#ifndef __zhCatmullRomSpline_h__
#define __zhCatmullRomSpline_h__

#include "zhPrereq.h"
#include "zhMathMacros.h"
#include "zhVector.h"
#include "zhMatrix.h"
#include "zhVector3.h"
#include "zhVector2.h"
#include "zhQuat.h"

namespace zh
{

/**
* @brief Generic implementation of a Catmull-Rom spline,
* suitable for interpolating between key-frame values.
*/
template <typename T>
class CatmullRomSpline
{

public:

	/**
	* Constructor.
	*/
	CatmullRomSpline()
	{
		mCoeffs = Matrix(4);
		mCoeffs.set( 0, 0, 2 );
		mCoeffs.set( 0, 1, -2 );
		mCoeffs.set( 0, 2, 1 );
		mCoeffs.set( 0, 3, 1 );
		mCoeffs.set( 1, 0, -3 );
		mCoeffs.set( 1, 1, 3 );
		mCoeffs.set( 1, 2, -2 );
		mCoeffs.set( 1, 3, -1 );
		mCoeffs.set( 2, 0, 0 );
		mCoeffs.set( 2, 1, 0 );
		mCoeffs.set( 2, 2, 1 );
		mCoeffs.set( 2, 3, 0 );
		mCoeffs.set( 3, 0, 1 );
		mCoeffs.set( 3, 1, 0 );
		mCoeffs.set( 3, 2, 0 );
		mCoeffs.set( 3, 3, 0 );
	}

	/**
	* Destructor.
	*/
	~CatmullRomSpline()
	{
	}

	/**
	* Adds a control point to the spline.
	*/
	void addControlPoint( const T& pt )
	{
		mCtrlPoints.push_back(pt);
	}

	/**
	* Clears all control points on the spline.
	*/
	void clearControlPoints()
	{
		mCtrlPoints.clear();
	}

	/**
	* Gets a control point on the spline.
	*/
	const T& getControlPoint( unsigned int index ) const
	{
		zhAssert( index < getNumControlPoints() );

		return mCtrlPoints[index];
	}

	/**
	* Sets a control point on the spline
	* (remember to call calcTangents afterwards).
	*/
	void setControlPoint( unsigned int index, const T& pt )
	{
		zhAssert( index < getNumControlPoints() );

		mCtrlPoints[index] = pt;
	}

	/**
	* Gets the tangent at the specified control point.
	*/
	const T& getTangentAtCPt( unsigned int index ) const
	{
		zhAssert( index < getNumControlPoints() );
		zhAssert( mTangents.size() == getNumControlPoints() );

		return mTangents[index];
	}

	/**
	* Gets the number of control points on the spline.
	*/
	unsigned int getNumControlPoints() const
	{
		return mCtrlPoints.size();
	}

	/**
	* Gets a point on the spline computed
	* by interpolating between control points nearest to the
	* value of the t parameter (normalized to range 0-1).
	* Points are assumed to be evenly spaced, so this should *not*
	* be used for interpolating between key-frames.
	*
	* @param t Interpolation parameter.
	* @return Interpolated point.
	*/
	T getPoint( float t ) const
	{
		// compute left-hand control point index
        float fcpi = t * ( float )( mCtrlPoints.size() - 1 );
        unsigned int cpi = ( unsigned int )fcpi;
		if( cpi >= mCtrlPoints.size() ) cpi = mCtrlPoints.size() - 1;
        
		// compute interp. param.
        t = fcpi - ( float )cpi;

        return getPoint( cpi, t );
	}

	/**
	* Gets a point on the spline computed
	* by interpolating between the control point at the specified index
	* and its right-hand neighbor.
	*
	* param index Index of the left-hand control point.
	* @param t Interpolation parameter.
	* @return Interpolated point.
	*/
	T getPoint( unsigned int index, float t ) const
	{
		zhAssert( mTangents.size() == getNumControlPoints() );

		if( index >= mCtrlPoints.size() - 1 )
			return mCtrlPoints[ mCtrlPoints.size() - 1 ];

		if( zhEqualf( t, 0 ) )
			return mCtrlPoints[index];
		else if( zhEqualf( t, 1 ) )
			return mCtrlPoints[index+1];

		float t2 = t*t;
		Vector tv(4);
		tv.set( 0, t2*t );
		tv.set( 1, t2 );
		tv.set( 2, t );
		tv.set( 3, 1 );
		tv *= mCoeffs;

		const T& cpt1 = mCtrlPoints[index];
		const T& cpt2 = mCtrlPoints[index+1];
		const T& tan1 = mTangents[index];
		const T& tan2 = mTangents[index+1];
		T pt = cpt1;

		for( unsigned int ei = 0; ei < cpt1.size(); ++ei )
		{
			pt.set( ei, tv.get(0) * cpt1.get(ei) +
				tv.get(1) * cpt2.get(ei) +
				tv.get(2) * tan1.get(ei) +
				tv.get(3) * tan2.get(ei) );
		}

		return pt;
	}

	/**
	* Gets a tangent on the spline computed
	* by interpolating between tangents at the control points
	* nearest to the value of the t parameter (normalized to range 0-1).
	* Points are assumed to be evenly spaced, so this should *not*
	* be used for interpolating between key-frames.
	*
	* @param t Interpolation parameter.
	* @return Interpolated tangent.
	*/
	T getTangent( float t ) const
	{
		// compute left-hand control point index
        float fcpi = t * ( float )( mCtrlPoints.size() - 1 );
        unsigned int cpi = ( unsigned int )fcpi;
		if( cpi >= mCtrlPoints.size() ) cpi = mCtrlPoints.size() - 1;
        
		// compute interp. param.
        t = fcpi - ( float )cpi;

        return getTangent( cpi, t );
	}

	/**
	* Gets a tangent on the spline computed
	* by interpolating between the tangent at the control point
	* at the specified index and its right-hand neighbor.
	*
	* param index Index of the left-hand control point.
	* @param t Interpolation parameter.
	* @return Interpolated tangent.
	*/
	T getTangent( unsigned int index, float t) const
	{
		zhAssert( mTangents.size() == getNumControlPoints() );

		if( index >= mCtrlPoints.size() - 1 )
			return mTangents[ mCtrlPoints.size() - 1 ];

		if( zhEqualf( t, 0 ) )
			return mTangents[index];
		else if( zhEqualf( t, 1 ) )
			return mTangents[index+1];

		float t2 = t*t;
		Vector tv(4);
		tv.set( 0, 3.f*t2 );
		tv.set( 1, 2.f*t );
		tv.set( 2, 1 );
		tv.set( 3, 0 );
		tv *= mCoeffs;

		const T& cpt1 = mCtrlPoints[index];
		const T& cpt2 = mCtrlPoints[index+1];
		const T& tan1 = mTangents[index];
		const T& tan2 = mTangents[index+1];
		T tang = cpt1;

		for( unsigned int ei = 0; ei < tang.size(); ++ei )
		{
			tang.set( ei, tv.get(0) * cpt1.get(ei) +
				tv.get(1) * cpt2.get(ei) +
				tv.get(2) * tan1.get(ei) +
				tv.get(3) * tan2.get(ei) );
		}

		return tang;
	}

	/**
	* Calculates tangents in all control points.
	* This should be called every time control points are edited.
	*/
	void calcTangents()
	{
		if( mTangents.size() == getNumControlPoints() )
			return;

		unsigned int ncpts = getNumControlPoints();
		if( ncpts < 2 )
			return;

		bool closed = mCtrlPoints[0] == mCtrlPoints[ncpts-1] ? 
			true : false;

		mTangents.resize(ncpts);

		for( unsigned int cpi = 0; cpi < ncpts; ++cpi )
		{
			if( cpi == 0 )
			{
				if(closed)
					mTangents[cpi] = ( mCtrlPoints[1] - mCtrlPoints[ncpts-2] ) * 0.5f;
				else
					mTangents[cpi] = ( mCtrlPoints[1] - mCtrlPoints[0] ) * 0.5f;
			}
			else if( cpi == ncpts - 1 )
			{
				if( closed )
					mTangents[cpi] = mTangents[0];
				else
					mTangents[cpi] = ( mCtrlPoints[cpi] - mCtrlPoints[cpi-1] ) * 0.5f;
			}
			else
			{
				mTangents[cpi] = ( mCtrlPoints[cpi+1] - mCtrlPoints[cpi-1] ) * 0.5f;
			}
		}
	}

private:

	Matrix mCoeffs;
	std::vector<T> mCtrlPoints;
	std::vector<T> mTangents;

};

/**
* @brief Specialization of CatmullRomSpline for quaternions,
* suitable for interpolating between rotations.
*/
template <>
class CatmullRomSpline<Quat>
{

public:

	/**
	* Constructor.
	*/
	CatmullRomSpline()
	{
	}

	/**
	* Destructor.
	*/
	~CatmullRomSpline()
	{
	}

	/**
	* Adds a control point to the spline.
	*/
	void addControlPoint( const Quat& pt )
	{
		mCtrlPoints.push_back(pt);
	}

	/**
	* Clears all control points on the spline.
	*/
	void clearControlPoints()
	{
		mCtrlPoints.clear();
	}

	/**
	* Gets a control point on the spline.
	*/
	const Quat& getControlPoint( unsigned int index ) const
	{
		zhAssert( index < getNumControlPoints() );

		return mCtrlPoints[index];
	}

	/**
	* Sets a control point on the spline
	* (remember to call calcTangents afterwards).
	*/
	void setControlPoint( unsigned int index, const Quat& pt )
	{
		zhAssert( index < getNumControlPoints() );

		mCtrlPoints[index] = pt;
	}

	/**
	* Gets the tangent at the specified control point.
	*/
	const Quat& getTangentAtCPt( unsigned int index ) const
	{
		zhAssert( index < getNumControlPoints() );
		zhAssert( mTangents.size() == getNumControlPoints() );

		return mTangents[index];
	}

	/**
	* Gets the number of control points on the spline.
	*/
	unsigned int getNumControlPoints() const
	{
		return mCtrlPoints.size();
	}

	/**
	* Gets a point on the spline computed
	* by interpolating between control points nearest to the
	* value of the t parameter (normalized to range 0-1).
	* Points are assumed to be evenly spaced, so this should *not*
	* be used for interpolating between key-frames.
	*
	* @param t Interpolation parameter.
	* @return Interpolated point.
	*/
	Quat getPoint( float t ) const
	{
		// compute left-hand control point index
        float fcpi = t * ( float )( mCtrlPoints.size() - 1 );
        unsigned int cpi = ( unsigned int )fcpi;
        
		// compute interp. param.
        t = fcpi - ( float )cpi;

        return getPoint( cpi, t );
	}

	/**
	* Gets a point on the spline computed
	* by interpolating between the control point at the specified index
	* and its right-hand neighbor.
	*
	* param index Index of the left-hand control point.
	* @param t Interpolation parameter.
	* @return Interpolated point.
	*/
	Quat getPoint( unsigned int index, float t) const
	{
		zhAssert( mTangents.size() == getNumControlPoints() );

		if( index >= mCtrlPoints.size() - 1 )
			return mCtrlPoints[ mCtrlPoints.size() - 1 ];

		if( zhEqualf( t, 0 ) )
			return mCtrlPoints[index];
		else if( zhEqualf( t, 1 ) )
			return mCtrlPoints[index+1];

		const Quat& cpt1 = mCtrlPoints[index];
		const Quat& cpt2 = mCtrlPoints[index+1];
		const Quat& tan1 = mTangents[index];
		const Quat& tan2 = mTangents[index+1];

		return cpt1.squad( tan1, tan2, cpt2, t );
	}

	/**
	* Gets a tangent on the spline computed
	* by interpolating between tangents at the control points
	* nearest to the value of the t parameter (normalized to range 0-1).
	* Points are assumed to be evenly spaced, so this should *not*
	* be used for interpolating between key-frames.
	*
	* @param t Interpolation parameter.
	* @return Interpolated tangent.
	*/
	Quat getTangent( float t ) const
	{
		// compute left-hand control point index
        float fcpi = t * ( float )( mCtrlPoints.size() - 1 );
        unsigned int cpi = ( unsigned int )fcpi;
        
		// compute interp. param.
        t = fcpi - ( float )cpi;

        return getTangent( cpi, t );
	}

	/**
	* Gets a tangent on the spline computed
	* by interpolating between the tangent at the control point
	* at the specified index and its right-hand neighbor.
	*
	* param index Index of the left-hand control point.
	* @param t Interpolation parameter.
	* @return Interpolated tangent.
	*/
	Quat getTangent( unsigned int index, float t) const
	{
		zhAssert( mTangents.size() == getNumControlPoints() );

		// TODO: how to implement this? (who needs this, anyway?)
		return Quat();
	}

	/**
	* Calculates tangents in all control points.
	* This should be called every time control points are edited.
	*/
	void calcTangents()
	{
		if( mTangents.size() == getNumControlPoints() )
			return;

		unsigned int ncpts = getNumControlPoints();
		if( ncpts < 2 )
			return;

		bool closed = mCtrlPoints[0] == mCtrlPoints[ncpts-1] ? 
			true : false;

		mTangents.resize(ncpts);

		Quat inv_cp, qlog1, qlog2, qlog;
		for( unsigned int cpi = 0; cpi < ncpts; ++cpi )
		{
			inv_cp = mCtrlPoints[cpi].getInverse();

			if( cpi == 0 )
			{
				qlog1 = inv_cp * mCtrlPoints[cpi+1];
				qlog1 = qlog1.log();

				if(closed)
					qlog2 = inv_cp * mCtrlPoints[ncpts-2];
				else
					qlog2 = inv_cp * mCtrlPoints[cpi];

				qlog2 = qlog2.log();
			}
			else if( cpi == ncpts - 1 )
			{
				if( closed )
					qlog1 = inv_cp * mCtrlPoints[1];
				else
					qlog1 = inv_cp * mCtrlPoints[cpi];

				qlog1 = qlog1.log();
				
				qlog2 = inv_cp * mCtrlPoints[cpi-1];
				qlog2 = qlog2.log();
			}
			else
			{
				qlog1 = inv_cp * mCtrlPoints[cpi+1];
				qlog1 = qlog1.log();

				qlog2 = inv_cp * mCtrlPoints[cpi-1];
				qlog2 = qlog2.log();
			}

			qlog = ( qlog1 + qlog2 ) * -0.25f;
			mTangents[cpi] = mCtrlPoints[cpi] * qlog.exp();
		}
	}

private:

	std::vector<Quat> mCtrlPoints;
	std::vector<Quat> mTangents;

};

}

#endif // __zhCatmullRomSpline_h__
