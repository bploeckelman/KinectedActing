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

#include "AnimationTrack.h"
#include "Util/zhCatmullRomSpline.h"
#include "Util/zhQuat.h"
#include "Util/zhVector3.h"
#include "Skeleton.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Animation;


/**
* @brief Class representing a bone animation track.
*/
class BoneAnimationTrack : public AnimationTrack
{

public:

	/**
	* Constructor.
	*
	* @param boneId Bone ID.
	* @param anim Pointer to the owning animation.
	*/
	BoneAnimationTrack( unsigned short boneId, Animation* anim );

	/**
	* Destructor.
	*/
	~BoneAnimationTrack();

	/**
	* Gets the ID of the bone affected by this track.
	*/
	unsigned short getBoneId() const;

	/**
	* Gets the key-frame interpolated from the neighboring key-frames
	* at the specified time.
	*
	* @param time Track sampling time.
	* @param kf Pointer to the interpolated key-frame.
	*/
	void getInterpolatedKeyFrame( float time, KeyFrame* kf ) const;

	/**
	* Applies the animation track to the specified skeleton.
	*
	* @param skel Pointer to the Skeleton
	* which should be updated by this animation track.
	* @param time Time at which this animation track should be applied.
	* @param weight Blend weight with which this animation track
	* should be applied.
	* @param scale Scaling factor applied to this animation track.
	*/
	 void apply( Skeleton* skel, float time, float weight = 1.f, float scale = 1.f ) const;

	 /**
	 * Builds the splines used for key-frame interpolation.
	 *
	 * @remark This function is called automatically by Animation.
	 * Do not call it manually without a good reason.
	 */
	 void _buildInterpSplines() const;

protected:

	KeyFrame* _createKeyFrame( float time );

private:

	unsigned short mBoneId;

	mutable zh::CatmullRomSpline<zh::Vector3> mTransSpline;//glm::vec3> mTransSpline;
	mutable zh::CatmullRomSpline<zh::Quat> mRotSpline;//glm::quat> mRotSpline;
	mutable zh::CatmullRomSpline<zh::Quat> mAbsRotSpline;//glm::quat> mAbsRotSpline;
	mutable zh::CatmullRomSpline<zh::Vector3> mScalSpline;//glm::vec3> mScalSpline;

};


inline unsigned short BoneAnimationTrack::getBoneId() const { return mBoneId; }
