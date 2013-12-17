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
#include "AnimationUtils.h"
#include "Util/zhQuat.h"
#include "Util/zhVector3.h"
#include "Util/zhMathMacros.h"

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
	if( zhEqualf( t, 0 ) )
	{
		tkf->setTranslation( tkf1->getTranslation() );
		tkf->setRotation( tkf1->getRotation() );
		tkf->setAbsRotation( tkf1->getAbsRotation() );
		tkf->setScale( tkf1->getScale() );
	}
	else if( zhEqualf( t, 1 ) )
	{
		tkf->setTranslation( tkf2->getTranslation() );
		tkf->setRotation( tkf2->getRotation() );
		tkf->setAbsRotation( tkf2->getAbsRotation() );
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
			tkf->setRotation( glm::slerp(q1, q2, t) );

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

			zh::Vector3 tr( mTransSpline.getPoint( tkf1->getIndex(), t) );
			zh::Quat r( mRotSpline.getPoint( tkf1->getIndex(), t) );
			zh::Quat a( mAbsRotSpline.getPoint( tkf1->getIndex(), t) );
			zh::Vector3 s( mScalSpline.getPoint( tkf1->getIndex(), t) );

			tkf->setTranslation( glm::vec3(tr.x, tr.y, tr.z) );//mTransSpline.getPoint( tkf1->getIndex(), t ) );
			tkf->setRotation( glm::quat(r.w, r.x, r.y, r.z) );//mRotSpline.getPoint( tkf1->getIndex(), t ) );
			tkf->setAbsRotation( glm::quat(a.w, a.x, a.y, a.z) );
			tkf->setScale( glm::vec3(s.x, s.y, s.z) );//mScalSpline.getPoint( tkf1->getIndex(), t ) );
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
	bone->scale = ( glm::vec3(1) + ( glm::vec3(1) - tkf.getScale() ) * weight * scale );
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

		glm::vec3 t(tkf->getTranslation());
		glm::quat r(tkf->getRotation());
		glm::quat a(tkf->getAbsRotation());
		glm::vec3 s(tkf->getScale());

		mTransSpline.addControlPoint( zh::Vector3(t.x, t.y, t.z) );//tkf->getTranslation() );
		mRotSpline.addControlPoint( zh::Quat(r.w, r.x, r.y, r.z) );//tkf->getRotation() );
		mAbsRotSpline.addControlPoint( zh::Quat(a.w, a.x, a.y, a.z) );//tkf->getAbsRotation() );
		mScalSpline.addControlPoint( zh::Vector3(s.x, s.y, s.z) );//tkf->getScale() );
	}

	mTransSpline.calcTangents();
	mRotSpline.calcTangents();
	mAbsRotSpline.calcTangents();
	mScalSpline.calcTangents();
}
