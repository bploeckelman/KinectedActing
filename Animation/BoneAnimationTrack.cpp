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
#include "BoneAnimationTrack.h"
#include "TransformKeyFrame.h"
#include "Animation.h"
#include "Skeleton.h"

#include <cassert>


BoneAnimationTrack::BoneAnimationTrack( unsigned short boneId, Animation* anim )
	: AnimationTrack(anim)
	, mBoneId(boneId)
{}

BoneAnimationTrack::~BoneAnimationTrack()
{}


void BoneAnimationTrack::getInterpolatedKeyFrame( float time, KeyFrame* kf ) const
{
	assert( kf != nullptr );

	TransformKeyFrame* tkf = static_cast<TransformKeyFrame*>(kf);
	KeyFrame *kf1, *kf2;
	TransformKeyFrame *tkf1, *tkf2;

	// get nearest 2 key-frames
	float t = getKeyFramesAtTime( time, &kf1, &kf2 );
	tkf1 = static_cast<TransformKeyFrame*>(kf1);
	tkf2 = static_cast<TransformKeyFrame*>(kf2);

	// interpolate between them
	//if( zhEqualf( t, 0 ) )
	if( fabs(t - 0) < 0.00001f )
	{
		tkf->setTranslation( tkf1->getTranslation() );
		tkf->setRotation( tkf1->getRotation() );
		tkf->setScale( tkf1->getScale() );
	}
	//else if( zhEqualf( t, 1 ) )
	else if ( fabs(t - 1) < 0.00001f )
	{
		tkf->setTranslation( tkf2->getTranslation() );
		tkf->setRotation( tkf2->getRotation() );
		tkf->setScale( tkf2->getScale() );
	}
	else
	{
		glm::vec3 v1, v2;
		glm::quat q1, q2;

		// interpolate transformations
		if( mAnim->getKFInterpMethod() == KFInterp_Linear )
		{
			v1 = tkf1->getTranslation();
			v2 = tkf2->getTranslation();
			tkf->setTranslation( v1 + ( v2 - v1 ) * t );

			q1 = tkf1->getRotation();
			q2 = tkf2->getRotation();
			tkf->setRotation( glm::lerp(q1, q2, t) );

			v1 = tkf1->getScale();
			v2 = tkf2->getScale();
			tkf->setScale( v1 + ( v2 - v1 ) * t );
		}
		else // if( mAnim->getKFInterpolationMethod() == KFInterp_Spline )
		{
			// TODO : spline interpolation is broken
			if( mTransSpline.getNumControlPoints() <= 0 )
				// interpolation splines not built yet, build them now
				_buildInterpSplines();

			tkf->setTranslation( mTransSpline.getPoint( tkf1->getIndex(), t ) );
			tkf->setRotation( mRotSpline.getPoint( tkf1->getIndex(), t ) );
			tkf->setScale( mScalSpline.getPoint( tkf1->getIndex(), t ) );
		}
	}
}

void BoneAnimationTrack::apply( Skeleton* skel, float time, float weight, float scale ) const
{
	TransformKeyFrame tkf( time, 0 );
	Bone* bone = skel->getBone(mBoneId);
	if( nullptr == bone )
		// Skeleton doesn't contain a bone for this track
		return;

	getInterpolatedKeyFrame( time, &tkf );

	bone->translation = glm::vec3( tkf.getTranslation() * weight * scale );
	bone->rotation = glm::slerp( glm::quat(), tkf.getRotation(), weight );
	bone->scale = ( glm::vec3(1,1,1) + ( glm::vec3(1,1,1) - tkf.getScale() ) * weight * scale );
}

KeyFrame* BoneAnimationTrack::_createKeyFrame( float time )
{
	return new TransformKeyFrame( time, 0 );
}

void BoneAnimationTrack::_buildInterpSplines() const
{
	TransformKeyFrame* tkf;

	for( unsigned int kfi = 0; kfi < mKeyFrames.size(); ++kfi )
	{
		tkf = static_cast<TransformKeyFrame*>( mKeyFrames[kfi] );
		mTransSpline.addControlPoint( tkf->getTranslation() );
		mRotSpline.addControlPoint( tkf->getRotation() );
		mScalSpline.addControlPoint( tkf->getScale() );
	}

	mTransSpline.calcTangents();
	mRotSpline.calcTangents();
	mScalSpline.calcTangents();
}
