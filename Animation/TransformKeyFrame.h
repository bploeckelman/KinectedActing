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
#include "KeyFrame.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


/**
* @brief Class representing a transform (translation, rotation, scale)
* key-frame.
*/
class TransformKeyFrame : public KeyFrame
{

public:

	/**
	* Constructor.
	*/
	TransformKeyFrame( float time, unsigned int index )
		: KeyFrame(time, index), mTranslation(), mRotation(), mAbsRotation(), mScale(1)
	{}

	/**
	* Destructor.
	*/
	~TransformKeyFrame() {}

	/**
	* Gets the translation component.
	*/
	const glm::vec3& getTranslation() const;

	/**
	* Sets the translation component.
	*/
	void setTranslation( const glm::vec3& trans );

	/**
	* Gets the rotation component.
	*/
	const glm::quat& getRotation() const;
	const glm::quat& getAbsRotation() const;

	/**
	* Sets the rotation component.
	*/
	void setRotation( const glm::quat& rot );
	void setAbsRotation( const glm::quat& rot );

	/**
	* Gets the scale component.
	*/
	const glm::vec3& getScale() const;

	/**
	* Gets the scale component.
	*/
	void setScale( const glm::vec3& scal );

private:

	glm::vec3 mTranslation;
	glm::quat mRotation;
	glm::quat mAbsRotation;
	glm::vec3 mScale;

};


inline const glm::vec3& TransformKeyFrame::getTranslation() const { return mTranslation; }
inline const glm::quat& TransformKeyFrame::getRotation() const { return mRotation; }
inline const glm::quat& TransformKeyFrame::getAbsRotation() const { return mAbsRotation; }
inline const glm::vec3& TransformKeyFrame::getScale() const { return mScale; }

inline void TransformKeyFrame::setTranslation( const glm::vec3& trans ) { mTranslation = trans; }
inline void TransformKeyFrame::setRotation( const glm::quat& rot ) { mRotation = rot; }
inline void TransformKeyFrame::setAbsRotation( const glm::quat& rot ) { mAbsRotation = rot; }
inline void TransformKeyFrame::setScale( const glm::vec3& scal ) { mScale = scal; }
