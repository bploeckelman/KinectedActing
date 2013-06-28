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
#include "AnimationTrack.h"
#include "Animation.h"
#include "KeyFrame.h"

#include <algorithm>
#include <cassert>


AnimationTrack::AnimationTrack( Animation* anim )
	: mKeyFrames()
	, mAnim(anim)
{}

AnimationTrack::~AnimationTrack()
{
	deleteAllKeyFrames();
}

KeyFrame* AnimationTrack::createKeyFrame( float time )
{
	KeyFrame* kf = nullptr;

	for( KeyFrameIterator kfi = begin(mKeyFrames); kfi != end(mKeyFrames); ++kfi )
	{
		//if( zhEqualf( (*kfi)->getTime(), time ) )
		if ((fabs((*kfi)->getTime() - time)) < 0.00001f)
		{
			kf = *kfi;
			break;
		}
		else if( (*kfi)->getTime() > time )
		{
			kf = _createKeyFrame(time);
			mKeyFrames.insert( kfi, kf );
			break;
		}
	}

	if( kf == nullptr )
	{
		kf = _createKeyFrame(time);
		mKeyFrames.push_back(kf);
	}

	_updateKeyFrameIndices();

	return kf;
}

void AnimationTrack::deleteKeyFrame( unsigned int index )
{
	assert( index < getNumKeyFrames() );

	delete mKeyFrames[index];
	mKeyFrames.erase( mKeyFrames.begin() + index );

	_updateKeyFrameIndices();
}

void AnimationTrack::deleteAllKeyFrames()
{
	for( unsigned int kfi = 0; kfi < mKeyFrames.size(); ++kfi )
		delete mKeyFrames[kfi];

	mKeyFrames.clear();
}

KeyFrame* AnimationTrack::getKeyFrame( unsigned int index ) const
{
	assert( index < getNumKeyFrames() );

	return mKeyFrames[index];
}

unsigned int AnimationTrack::getNumKeyFrames() const
{
	return mKeyFrames.size();
}

AnimationTrack::KeyFrameIterator AnimationTrack::getKeyFrameIterator()
{
	return begin(mKeyFrames); //KeyFrameIterator( mKeyFrames );
}

AnimationTrack::KeyFrameConstIterator AnimationTrack::getKeyFrameConstIterator() const
{
	return begin(mKeyFrames); //KeyFrameConstIterator( mKeyFrames );
}

float AnimationTrack::getKeyFramesAtTime( float time, KeyFrame** kf1, KeyFrame** kf2 ) const
{
	float t = 0;
	*kf1 = *kf2 = nullptr;

	if( mKeyFrames.size() == 0 )
		return 0;

	KeyFrameCompare kfc;
	KeyFrameConstIterator kfi = std::upper_bound( begin(mKeyFrames), end(mKeyFrames), time, kfc );

	if( kfi == begin(mKeyFrames) )
	{
		*kf1 = *kf2 = *kfi;
	}
	else if( kfi == end(mKeyFrames) )
	{
		*kf1 = *kf2 = *( kfi - 1 );
	}
	else
	{
		*kf1 = *( kfi - 1 );
		*kf2 = *kfi;
		t = ( time - (*kf1)->getTime() ) / ( (*kf2)->getTime() - (*kf1)->getTime() );
	}

	return t;
}

float AnimationTrack::getLength() const
{
	if( getNumKeyFrames() <= 0 )
		return 0;

	return mKeyFrames[ getNumKeyFrames() - 1 ]->getTime();
}

void AnimationTrack::_updateKeyFrameIndices()
{
	for( unsigned int kfi = 0; kfi < mKeyFrames.size(); ++kfi )
		mKeyFrames[kfi]->_setIndex(kfi);
}
