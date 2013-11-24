#pragma once

#include "AnimationTypes.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>
#include <set>

class BoneAnimationTrack;
enum EBoneID;

enum KFInterpMethod
{
	KFInterp_Linear,
	KFInterp_Spline
};

typedef std::map<unsigned short, BoneAnimationTrack*> BoneTracks; 
typedef BoneTracks::iterator                          BoneTrackIterator;
typedef BoneTracks::const_iterator                    BoneTrackConstIterator;


class Animation
{
	friend class Skeleton;

public:
	Animation(unsigned short id, const std::string& name);
	~Animation();

	void apply(Skeleton* skel, float time, float weight=1.f, float scale=1.f, const BoneMask& boneMask=default_bone_mask) const;
	void getPositions(unsigned short boneId, std::vector<glm::vec3>& positions, float lastTime=-1.f) const;

	void deleteAllBoneTrack();
	void deleteBoneTrack(unsigned short boneId);
	BoneAnimationTrack* createBoneTrack(unsigned short boneId);

	float getLength() const;
	unsigned short getId() const;
	const std::string& getName() const;
	const BoneTracks& getBoneTracks() const;
	KFInterpMethod getKFInterpMethod() const;
	BoneAnimationTrack* getBoneTrack(unsigned short boneId) const;

	void setKFInterpMethod(KFInterpMethod interpMethod);

	size_t _calcMemoryUsage() const;
	void _clone(Animation* clonePtr) const;

private:
	unsigned short mId;
	std::string mName;

	BoneTracks mBoneTracks;
	KFInterpMethod mInterpMethod;

};


inline unsigned short Animation::getId() const { return mId; }
inline const std::string& Animation::getName() const { return mName; }
inline KFInterpMethod Animation::getKFInterpMethod() const { return mInterpMethod; }
inline const BoneTracks& Animation::getBoneTracks() const { return mBoneTracks; }
