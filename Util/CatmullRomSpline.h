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
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <cassert>


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
		: mCoeffs(glm::mat4())
	{
		mCoeffs[0][0] =  2.f; mCoeffs[0][1] = -2.f; mCoeffs[0][2] =  1.f; mCoeffs[0][3] =  1.f;
		mCoeffs[1][0] = -3.f; mCoeffs[1][1] =  3.f; mCoeffs[1][2] = -2.f; mCoeffs[1][3] = -1.f;
		mCoeffs[2][0] =  0.f; mCoeffs[2][1] =  0.f; mCoeffs[2][2] =  1.f; mCoeffs[2][3] =  1.f;
		mCoeffs[3][0] =  1.f; mCoeffs[3][1] =  0.f; mCoeffs[3][2] =  0.f; mCoeffs[3][3] =  0.f;
	}

	/**
	* Destructor.
	*/
	~CatmullRomSpline()
	{}

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
		assert( index < getNumControlPoints() );

		return mCtrlPoints[index];
	}

	/**
	* Sets a control point on the spline
	* (remember to call calcTangents afterwards).
	*/
	void setControlPoint( unsigned int index, const T& pt )
	{
		assert( index < getNumControlPoints() );

		mCtrlPoints[index] = pt;
	}

	/**
	* Gets the tangent at the specified control point.
	*/
	const T& getTangentAtCPt( unsigned int index ) const
	{
		assert( index < getNumControlPoints() );
		assert( mTangents.size() == getNumControlPoints() );

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
		assert( mTangents.size() == getNumControlPoints() );

		if( index >= mCtrlPoints.size() - 1 )
			return mCtrlPoints[ mCtrlPoints.size() - 1 ];

		//if( zhEqualf( t, 0 ) )
		if( fabs(t - 0) < 0.00001f )
			return mCtrlPoints[index];
		//else if( zhEqualf( t, 1 ) )
		else if( fabs(t - 1) < 0.00001f )
			return mCtrlPoints[index+1];

		float t2 = t*t;
		glm::vec4 tv(t2*t, t2, t, 1);
		//tv *= mCoeffs;
		tv = tv * mCoeffs;

		const T& cpt1 = mCtrlPoints[index];
		const T& cpt2 = mCtrlPoints[index+1];
		const T& tan1 = mTangents[index];
		const T& tan2 = mTangents[index+1];
		T pt = cpt1;

		//for( unsigned int ei = 0; ei < cpt1.size(); ++ei )
		//{
		//	pt.set( ei, tv.get(0) * cpt1.get(ei) +
		//		tv.get(1) * cpt2.get(ei) +
		//		tv.get(2) * tan1.get(ei) +
		//		tv.get(3) * tan2.get(ei) );
		//}
		// NOTE: glm::quat has a specialized version of CatmullRomSpline, but glm::vecN doesn't
		// for now this is using a hardcoded size so that it can handle the translation/scaling 
		// portions of a TransformKeyFrame object
		for( unsigned int ei = 0; ei < 3; ++ei )
		{
			pt[ei] = tv[0] * cpt1[ei]
				   + tv[1] * cpt2[ei]
				   + tv[2] * tan1[ei]
				   + tv[3] * tan2[ei];
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
		
		// compute interpolation param.
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
		assert( mTangents.size() == getNumControlPoints() );

		if( index >= mCtrlPoints.size() - 1 )
			return mTangents[ mCtrlPoints.size() - 1 ];

		//if( zhEqualf( t, 0 ) )
		if( fabs(t - 0) < 0.00001f )
			return mTangents[index];
		//else if( zhEqualf( t, 1 ) )
		else if( fabs(t - 1) < 0.00001f )
			return mTangents[index+1];

		float t2 = t*t;
		glm::vec4 tv(3.f*t2, 2.f*t, 1.f, 0.f);
		tv *= mCoeffs;

		const T& cpt1 = mCtrlPoints[index];
		const T& cpt2 = mCtrlPoints[index+1];
		const T& tan1 = mTangents[index];
		const T& tan2 = mTangents[index+1];
		T tang = cpt1;

		//for( unsigned int ei = 0; ei < tang.size(); ++ei )
		//{
		//	tang.set( ei, tv.get(0) * cpt1.get(ei) +
		//		tv.get(1) * cpt2.get(ei) +
		//		tv.get(2) * tan1.get(ei) +
		//		tv.get(3) * tan2.get(ei) );
		//}
		// NOTE: glm::quat has a specialized version of CatmullRomSpline, but glm::vecN doesn't
		// for now this is using a hardcoded size so that it can handle the translation/scaling 
		// portions of a TransformKeyFrame object
		for( unsigned int ei = 0; ei < 3; ++ei )
		{
			tang[ei] = tv[0] * cpt1[ei]
				     + tv[1] * cpt2[ei]
					 + tv[2] * tan1[ei]
					 + tv[3] * tan2[ei];
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

	glm::mat4 mCoeffs;
	std::vector<T> mCtrlPoints;
	std::vector<T> mTangents;

};

/**
* @brief Specialization of CatmullRomSpline for quaternions,
* suitable for interpolating between rotations.
*/
template <>
class CatmullRomSpline<glm::quat>
{

public:

	/**
	* Constructor.
	*/
	CatmullRomSpline()
	{}

	/**
	* Destructor.
	*/
	~CatmullRomSpline()
	{}

	/**
	* Adds a control point to the spline.
	*/
	void addControlPoint( const glm::quat& pt )
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
	const glm::quat& getControlPoint( unsigned int index ) const
	{
		assert( index < getNumControlPoints() );

		return mCtrlPoints[index];
	}

	/**
	* Sets a control point on the spline
	* (remember to call calcTangents afterwards).
	*/
	void setControlPoint( unsigned int index, const glm::quat& pt )
	{
		assert( index < getNumControlPoints() );

		mCtrlPoints[index] = pt;
	}

	/**
	* Gets the tangent at the specified control point.
	*/
	const glm::quat& getTangentAtCPt( unsigned int index ) const
	{
		assert( index < getNumControlPoints() );
		assert( mTangents.size() == getNumControlPoints() );

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
	glm::quat getPoint( float t ) const
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
	glm::quat getPoint( unsigned int index, float t) const
	{
		assert( mTangents.size() == getNumControlPoints() );

		if( index >= mCtrlPoints.size() - 1 )
			return mCtrlPoints[ mCtrlPoints.size() - 1 ];

		//if( zhEqualf( t, 0 ) )
		if( fabs(t - 0) < 0.00001f )
			return mCtrlPoints[index];
		//else if( zhEqualf( t, 1 ) )
		else if( fabs(t - 1) < 0.00001f )
			return mCtrlPoints[index+1];

		const glm::quat& cpt1 = mCtrlPoints[index];
		const glm::quat& cpt2 = mCtrlPoints[index+1];
		const glm::quat& tan1 = mTangents[index];
		const glm::quat& tan2 = mTangents[index+1];

		//return cpt1.squad( tan1, tan2, cpt2, t );
		// TODO : write glm adapter for Quat Quat::squad(const Quat& qa, const Quat& qb, const Quat& q, float t) const
		float t0 = 2.f * t - (1.f - t);
		glm::quat qi1 = glm::slerp(cpt1, cpt2, t);
		glm::quat qi2 = glm::slerp(tan1, tan2, t);
		return glm::slerp(qi1, qi2, t0);
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
	glm::quat getTangent( float t ) const
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
	glm::quat getTangent( unsigned int index, float t) const
	{
		assert( mTangents.size() == getNumControlPoints() );

		// TODO: how to implement this? (who needs this, anyway?)
		return glm::quat();
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

		glm::quat inv_cp, qlog1, qlog2, qlog;
		for( unsigned int cpi = 0; cpi < ncpts; ++cpi )
		{
			inv_cp = glm::inverse(mCtrlPoints[cpi]);

			if( cpi == 0 )
			{
				qlog1 = inv_cp * mCtrlPoints[cpi+1];
				qlog1 = quat_log(qlog1);

				if(closed)
					qlog2 = inv_cp * mCtrlPoints[ncpts-2];
				else
					qlog2 = inv_cp * mCtrlPoints[cpi];

				qlog2 = quat_log(qlog2);
			}
			else if( cpi == ncpts - 1 )
			{
				if( closed )
					qlog1 = inv_cp * mCtrlPoints[1];
				else
					qlog1 = inv_cp * mCtrlPoints[cpi];

				qlog1 = quat_log(qlog1);
				
				qlog2 = inv_cp * mCtrlPoints[cpi-1];
				qlog2 = quat_log(qlog2);
			}
			else
			{
				qlog1 = inv_cp * mCtrlPoints[cpi+1];
				qlog1 = quat_log(qlog1);

				qlog2 = inv_cp * mCtrlPoints[cpi-1];
				qlog2 = quat_log(qlog2);
			}

			qlog = ( qlog1 + qlog2 ) * -0.25f;
			mTangents[cpi] = mCtrlPoints[cpi] * quat_exp(qlog);
		}
	}

	glm::quat quat_log(const glm::quat& q)
	{
		glm::quat qout(q);

		if( fabs(qout.w) < 1.f )
		{
			float a = glm::acos(qout.w);
			float sin_a = glm::sin(a);

			if( fabs(sin_a) >= 0.005f )
			{
				float c = a / sin_a;
				qout.x *= c;
				qout.y *= c;
				qout.z *= c;
			}
		}

		qout.w = 0;

		return qout;
	}

	glm::quat quat_exp(const glm::quat& q)
	{
		glm::quat qout(q);

		float a = glm::sqrt(qout.x*qout.x + qout.y*qout.y + qout.z*qout.z);
		float sin_a = glm::sin(a);

		if( fabs(sin_a) >= 0.005f )
		{
			float c = sin_a / a;
			qout.x *= c;
			qout.y *= c;
			qout.z *= c;
		}

		qout.w = glm::cos(a);

		return qout;
	}

private:

	std::vector<glm::quat> mCtrlPoints;
	std::vector<glm::quat> mTangents;

};
