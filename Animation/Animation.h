#pragma once

#include <string>
#include <map>

class BoneAnimationTrack;

enum KFInterpolationMethod
{
	KFInterp_Linear,
	KFInterp_Spline
};


class Animation
{
	typedef std::map<unsigned short, BoneAnimationTrack*>::iterator       BoneTrackIterator;
	typedef std::map<unsigned short, BoneAnimationTrack*>::const_iterator BoneTrackConstIterator;

public:
	Animation(unsigned short id, const std::string& name /*, AnimationSetPtr animSet*/);
	~Animation();

	unsigned short getId() const;
	const std::string& getName() const;
	std::string getFullName() const;
	//AnimationSetPtr getAnimationSet() const;

	BoneAnimationTrack* createBoneTrack(unsigned short boneId);
	void deleteBoneTrack(unsigned short boneId);
	void deleteAllBoneTrack();
	bool hasBoneTrack(unsigned short boneId) const;
	BoneAnimationTrack* getBoneTrack(unsigned short boneId) const;
	BoneTrackIterator getBoneTrackIterator();
	BoneTrackConstIterator getBoneTrackConstIterator() const;

	KFInterpolationMethod getKFInterpolationMethod() const;
	void setKFInterpolationMethod(KFInterpolationMethod interpMethod);

	float getLength() const;
	int getFrameRate() const;
	void setFrameRate(int frameRate);

	//void apply(Skeleton* skel, float time, float weight=1.f, float scale=1.f, const std::set<unsigned short> boneMask=EmptyBoneMask) const;

	void computeAnimationBounds(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) const;

	size_t _calcMemoryUsage() const;
	void _clone(Animation* clonePtr) const;

private:
	unsigned short mId;
	std::string mName;
	int mFrameRate;

	//AnimationSetPtr mAnimSet;
	std::map<unsigned short, BoneAnimationTrack*> mBoneTracks;
	KFInterpolationMethod mInterpMethod;

};


inline unsigned short Animation::getId() const { return mId; }
inline const std::string& Animation::getName() const { return mName; }
//inline AnimationSetPtr getAnimationSet() const { return mAnimSet; }
inline KFInterpolationMethod Animation::getKFInterpolationMethod() const { return mInterpMethod; }

inline int Animation::getFrameRate() const { return mFrameRate; }
inline void Animation::setFrameRate(int frameRate) { mFrameRate = frameRate; } // TODO : assert(frameRate > 0)
