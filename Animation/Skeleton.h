#pragma once

#include "AnimationTypes.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <map>


class Bone
{
public:
	Bone(EBoneID boneId, EBoneID parentId, const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale)
		: boneId(boneId)
		, parentId(parentId)
		, translation(translation)
		, rotation(rotation)
		, scale(scale)
	{}

	const EBoneID boneId;
	const EBoneID parentId;

	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
};


class Skeleton
{
public:
	static const BoneJointPairs jointPairs;

	bool render_bones;
	bool render_joints;
	bool render_orientations;

	Skeleton();
	~Skeleton();

	void render() const;

	void renderBones() const;
	void renderJoints() const;
	void renderOrientations() const;

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
