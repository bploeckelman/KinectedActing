#include "Skeleton.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"
#include "Shaders/Program.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <map>

const float s = 0.025f;
const glm::vec3 scale(s);
const glm::vec3 zero(0);
const glm::vec3 y(0,1,0); // world up

const BoneJointPairs Skeleton::jointPairs([]() {
	BoneJointPairs bones;
	bones[SHOULDER_CENTER] = HEAD;
	bones[SHOULDER_LEFT  ] = SHOULDER_CENTER;
	bones[SHOULDER_RIGHT ] = SHOULDER_CENTER;
	bones[ELBOW_LEFT     ] = SHOULDER_LEFT;
	bones[ELBOW_RIGHT    ] = SHOULDER_RIGHT;
	bones[WRIST_LEFT     ] = ELBOW_LEFT;
	bones[WRIST_RIGHT    ] = ELBOW_RIGHT;
	bones[HAND_LEFT      ] = WRIST_LEFT;
	bones[HAND_RIGHT     ] = WRIST_RIGHT;
	bones[SPINE          ] = SHOULDER_CENTER;
	bones[HIP_CENTER     ] = SPINE;
	bones[HIP_LEFT       ] = HIP_CENTER;
	bones[HIP_RIGHT      ] = HIP_CENTER;
	bones[KNEE_LEFT      ] = HIP_LEFT;
	bones[KNEE_RIGHT     ] = HIP_RIGHT;
	bones[ANKLE_LEFT     ] = KNEE_LEFT;
	bones[ANKLE_RIGHT    ] = KNEE_RIGHT;
	bones[FOOT_LEFT      ] = ANKLE_LEFT;
	bones[FOOT_RIGHT     ] = ANKLE_RIGHT;
	return bones;
}());


Skeleton::Skeleton()
	: bones()
	, render_bones(true)
	, render_joints(true)
	, render_orientations(true)
{
	initBones();
}

Skeleton::~Skeleton()
{
	bones.clear();
}

void Skeleton::render() const
{
	if (render_bones)        renderBones();
	if (render_joints)       renderJoints();
	if (render_orientations) renderOrientations();
}

void Skeleton::renderBones() const
{
	glm::mat4 model;

	std::for_each(begin(jointPairs), end(jointPairs), [&](const BoneJointPairs::value_type& joints) {
		// Get the two joints for this bone
		const Bone& bone1 = bones.at(joints.first);
		const Bone& bone2 = bones.at(joints.second);

		if (bone1.translation != zero && bone2.translation != zero) {
			// Calculate orientation and position for cylinder connecting bone1 and bone2
			const float dist        = glm::distance(bone1.translation, bone2.translation);
			const glm::vec3 diff    = bone2.translation - bone1.translation;
			const glm::vec3 forward = glm::normalize(diff);
			const glm::vec3 axis    = glm::cross(y, forward);
			const float angle       = glm::degrees(acos(glm::dot(y, forward)));

			// Calculate the model matrix for this cylinder using the orientation and position
			model = glm::rotate(glm::translate(glm::mat4(), bone1.translation), angle, axis);
			model = glm::scale(model, glm::vec3(s,dist,s));
			GLUtils::defaultProgram->setUniform("model", model);

			Render::cylinder();
		}
	});
}

void Skeleton::renderJoints() const
{
	std::for_each(begin(bones), end(bones), [&](const std::pair<EBoneID, Bone>& pair) {
		const Bone& bone = pair.second;

		if (bone.translation != zero) {
			GLUtils::defaultProgram->setUniform("model",
				glm::scale(glm::translate(glm::mat4(), bone.translation), scale));
			Render::sphere();
		}
	});
}

void Skeleton::renderOrientations() const
{
	glm::mat4 model;

	std::for_each(begin(bones), end(bones), [&](const std::pair<EBoneID, Bone>& pair) {
		const Bone& bone = pair.second;
		if (bone.translation != zero) {
			// Calculate global rotation for this bone
			glm::quat globalRotation = bone.rotation;
			EBoneID boneID = bone.boneId;
			EBoneID parentID = bone.parentId;
			while (parentID != COUNT) {
				// accumulate parent's rotation
				globalRotation = bones.at(parentID).rotation * globalRotation;
				// move up the hierarchy
				boneID = parentID;
				parentID = bones.at(boneID).parentId;
			}

			model = glm::translate(glm::mat4(), bone.translation);
			model = model * glm::mat4_cast(globalRotation);
			model = glm::scale(model, glm::vec3(0.1));
			GLUtils::defaultProgram->setUniform("model", model);

			Render::axis();
		}
	});
}

void Skeleton::initBones() 
{
	for(int bone_id = EBoneID::HIP_CENTER; bone_id != EBoneID::COUNT; ++bone_id) {
		EBoneID boneID = static_cast<EBoneID>(bone_id);

		EBoneID parentID = HIP_CENTER;
		switch (boneID) {
			case HIP_CENTER:      parentID = COUNT;           break;
			case SPINE:           parentID = HIP_CENTER;      break;
			case SHOULDER_CENTER: parentID = SPINE;           break;
			case HEAD:            parentID = SHOULDER_CENTER; break;
			case SHOULDER_LEFT:   parentID = SHOULDER_CENTER; break;
			case ELBOW_LEFT:      parentID = SHOULDER_LEFT;   break;
			case WRIST_LEFT:      parentID = ELBOW_LEFT;      break;
			case HAND_LEFT:       parentID = WRIST_LEFT;      break;
			case SHOULDER_RIGHT:  parentID = SHOULDER_CENTER; break;
			case ELBOW_RIGHT:     parentID = SHOULDER_RIGHT;  break;
			case WRIST_RIGHT:     parentID = ELBOW_RIGHT;     break;
			case HAND_RIGHT:      parentID = WRIST_RIGHT;     break;
			case HIP_LEFT:        parentID = HIP_CENTER;      break;
			case KNEE_LEFT:       parentID = HIP_LEFT;        break;
			case ANKLE_LEFT:      parentID = KNEE_LEFT;       break;
			case FOOT_LEFT:       parentID = ANKLE_LEFT;      break;
			case HIP_RIGHT:       parentID = HIP_CENTER;      break;
			case KNEE_RIGHT:      parentID = HIP_RIGHT;       break;
			case ANKLE_RIGHT:     parentID = KNEE_RIGHT;      break;
			case FOOT_RIGHT:      parentID = ANKLE_RIGHT;     break;
			default:              parentID = COUNT;
		}

		Bone bone(boneID, parentID, glm::vec3(), glm::quat(), glm::vec3(1));
		bones.insert(std::make_pair(boneID, bone));
	}
}
