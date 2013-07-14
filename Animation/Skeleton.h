#pragma once

#include "Animation.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <map>


// Same order as Kinect NUI_SKELETON_POSITION_*
enum EBoneID
{
	HIP_CENTER      = 0,
	SPINE           = 1,
	SHOULDER_CENTER = 2,
	HEAD            = 3,
	SHOULDER_LEFT   = 4,
	ELBOW_LEFT      = 5,
	WRIST_LEFT      = 6,
	HAND_LEFT       = 7,
	SHOULDER_RIGHT  = 8,
	ELBOW_RIGHT     = 9,
	WRIST_RIGHT     = 10,
	HAND_RIGHT      = 11,
	HIP_LEFT        = 12,
	KNEE_LEFT       = 13,
	ANKLE_LEFT      = 14,
	FOOT_LEFT       = 15,
	HIP_RIGHT       = 16,
	KNEE_RIGHT      = 17,
	ANKLE_RIGHT     = 18,
	FOOT_RIGHT      = 19,
	COUNT           = 20
};

typedef std::map<EBoneID, EBoneID> BoneJointPairs;

class Bone
{
public:
	Bone(EBoneID boneId, const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale)
		: boneId(boneId)
		, translation(translation)
		, rotation(rotation)
		, scale(scale)
	{}

	const EBoneID boneId;
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
};


class Skeleton
{
public:
	static const BoneJointPairs jointPairs;

	Skeleton();
	~Skeleton();

	void render() const;

	Bone* getBone(unsigned short boneID);
	const Bone* getBone(unsigned short boneID) const;

private:
	void initBones();

private:
	std::map<EBoneID, Bone> bones;

};


inline Bone* Skeleton::getBone(unsigned short boneID)
{
	if( boneID >= EBoneID::COUNT ) {
		return nullptr;
	} else {
		return &bones.at(static_cast<EBoneID>(boneID));
	}
}

inline const Bone* Skeleton::getBone(unsigned short boneID) const
{
	if( boneID >= EBoneID::COUNT ) {
		return nullptr;
	} else {
		return &bones.at(static_cast<EBoneID>(boneID));
	}
}
