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

#include <vector>

class KeyFrame;
class Animation;


/**
* @brief Base class for animation tracks.
*/
class AnimationTrack
{

public:

	typedef std::vector<KeyFrame*>::iterator       KeyFrameIterator;
	typedef std::vector<KeyFrame*>::const_iterator KeyFrameConstIterator;

	/**
	* Constructor.
	*
	* @param id Animation track ID.
	* @param anim Pointer to the owning animation.
	*/
	AnimationTrack( Animation* anim );

	/**
	* Destructor.
	*/
	virtual ~AnimationTrack();

	/**
	* Gets a pointer to the owning animation.
	*/
	virtual Animation* getAnimation() const;

	/**
	* Gets the AnimationTrack class ID.
	*
	* This pure virtual function should be implemented in concrete
	* animation track classes.
	*/
	//virtual AnimationTrackClass getClassId() const = 0;

	/**
	* Creates a new animation key-frame.
	*
	* @param time Key-frame time.
	* @return Pointer to the key-frame.
	*/
	virtual KeyFrame* createKeyFrame( float time );

	/**
	* Deletes the key-frame at the specified index.
	* 
	* @param index Key-frame index.
	*/
	virtual void deleteKeyFrame( unsigned int index );

	/**
	* Deletes all key-frames.
	*/
	virtual void deleteAllKeyFrames();

	/**
	* Gets the key-frame at the specified index.
	* 
	* @param index Key-frame index.
	* @return Pointer to the key-frame.
	*/
	virtual KeyFrame* getKeyFrame( unsigned int index ) const;

	virtual const std::vector<KeyFrame*>& getKeyFrames() const;

	/**
	* Gets the number of key-frames in this animation track.
	*/
	virtual unsigned int getNumKeyFrames() const;

	/**
	* Gets an iterator over the vector of key-frames.
	*/
	virtual KeyFrameIterator getKeyFrameIterator();

	/**
	* Gets a const iterator over the vector of key-frames.
	*/
	virtual KeyFrameConstIterator getKeyFrameConstIterator() const;

	/**
	* Gets the two key-frames adjacent to the specified time.
	*
	* @param time Track sampling time.
	* @param kf1 Pointer to the first key-frame.
	* @param kf2 Pointer to the second key-frame.
	* @return Parameter t which indicates where the sampling time
	* point lies between the two key-frames (normalized 0-1 range is used).
	*/
	virtual float getKeyFramesAtTime( float time, KeyFrame** kf1, KeyFrame** kf2 ) const;

	/**
	* Gets the key-frame interpolated from the neighboring key-frames
	* at the specified time.
	*
	* @param time Track sampling time.
	* @param kf Pointer to the interpolated key-frame.
	*/
	virtual void getInterpolatedKeyFrame( float time, KeyFrame* kf ) const = 0;

	/**
	* Gets the length of the animation track.
	*/
	virtual float getLength() const;

	/**
	* Applies the animation track to the specified skeleton.
	*
	* This pure virtual function should be implemented in concrete
	* animation track classes.
	*
	* @param skeleton Pointer to the Skeleton
	* which should be updated by this animation track.
	* @param time Time at which this animation track should be applied.
	* @param weight Blend weight with which this animation track
	* should be applied.
	* @param scale Scaling factor applied to this animation track.
	*/
	//virtual void apply( Skeleton* skeleton, float time, float weight = 1.f, float scale = 1.f ) const = 0;

protected:

	virtual KeyFrame* _createKeyFrame( float time ) = 0; ///< Actual key-frame creation, implemented in concrete AnimationTrack subclasses.
	virtual void _updateKeyFrameIndices();

	Animation* mAnim;

	std::vector<KeyFrame*> mKeyFrames;

};


inline Animation* AnimationTrack::getAnimation() const { return mAnim; }
inline const std::vector<KeyFrame*>& AnimationTrack::getKeyFrames() const { return mKeyFrames; }
