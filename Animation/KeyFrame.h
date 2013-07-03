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


/**
* @brief Base class for animation key-frames.
*/
class KeyFrame
{
	friend class AnimationTrack;

public:
	/**
	* Constructor.
	*/
	KeyFrame( float time, unsigned int index ) : mTime(time), mIndex(index) {}

	/**
	* Destructor.
	*/
	virtual ~KeyFrame() {}

	/**
	* Gets the key-frame time.
	*/
	float getTime() const;

	/**
	* Gets the key-frame index.
	*/
	unsigned int getIndex() const;

protected:

	void _setIndex( unsigned int index );

	float mTime;
	unsigned int mIndex;

};


inline float KeyFrame::getTime() const { return mTime; }
inline unsigned int KeyFrame::getIndex() const { return mIndex; }
inline void KeyFrame::_setIndex( unsigned int index ) { mIndex = index; }


/**
 * @brief This struct is used for comparing key-frames by
 * key-frame time.
 */
struct KeyFrameCompare
{

public:

	bool operator()( const KeyFrame* kf1, const KeyFrame* kf2 );
	bool operator()( const KeyFrame* kf1, float t2 );
	bool operator()( float t1, const KeyFrame* kf2 );

};


inline bool KeyFrameCompare::operator()( const KeyFrame* kf1, const KeyFrame* kf2 )
{
	return kf1->getTime() < kf2->getTime();
}

inline bool KeyFrameCompare::operator()( const KeyFrame* kf1, float t2 )
{
	return kf1->getTime() < t2;
}

inline bool KeyFrameCompare::operator()( float t1, const KeyFrame* kf2 )
{
	return t1 < kf2->getTime();
}
