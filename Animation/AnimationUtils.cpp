#include "AnimationUtils.h"

#include "Skeleton.h"
#include "Animation.h"
#include "TransformKeyFrame.h"
#include "BoneAnimationTrack.h"
#include "Shaders/Program.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// TODO : most of this stuff should be moved to a refactored Skeleton class
const BoneJointPairs jointPairs([]() {
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
bool offsets_calculated = false;
glm::vec3 bone_offsets[20];
glm::mat4 world_transforms[20];

EBoneID getParentBoneID(const EBoneID& boneID)
{
	static EBoneID parents[20] = {
		HIP_CENTER,
		HIP_CENTER,
		SPINE,
		SHOULDER_CENTER,
		SHOULDER_CENTER,
		SHOULDER_LEFT,
		ELBOW_LEFT,
		WRIST_LEFT,
		SHOULDER_CENTER,
		SHOULDER_RIGHT,
		ELBOW_RIGHT,
		WRIST_RIGHT,
		HIP_CENTER,
		HIP_LEFT,
		KNEE_LEFT,
		ANKLE_LEFT,
		HIP_CENTER,
		HIP_RIGHT,
		KNEE_RIGHT,
		ANKLE_RIGHT
	};
	return parents[boneID];
}


void recalculateBoneOffsets(const Animation& animation, const float time) {
	const float scale = 10.f;

	TransformKeyFrame childKF(time, 0);
	TransformKeyFrame parentKF(time, 0);

	for (int childID = HIP_CENTER; childID < EBoneID::COUNT; ++childID) {
		int parentID = getParentBoneID((EBoneID) childID);

		animation.getBoneTrack(childID)->getInterpolatedKeyFrame(time, &childKF);
		animation.getBoneTrack(parentID)->getInterpolatedKeyFrame(time, &parentKF);

		bone_offsets[childID] = scale * (childKF.getTranslation() - parentKF.getTranslation());
	}
}

void renderJoint(const glm::mat4& model)
{
	GLUtils::defaultProgram->setUniform("model", model);
	GLUtils::defaultProgram->setUniform("useLighting", 1);
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
	GLUtils::defaultProgram->setUniform("tex", 0);
	Render::sphere();
}

void renderOrientation(const glm::mat4& model) {
	GLUtils::defaultProgram->setUniform("model", model);
	GLUtils::defaultProgram->setUniform("useLighting", 0);
	Render::axis();
}


void renderBones(const Animation& animation, const float time)
{
	const float s = 0.015f;
	glm::mat4 model;

	std::for_each(begin(jointPairs), end(jointPairs), [&](const BoneJointPairs::value_type& joints) {
		TransformKeyFrame b1kf(time, 0);
		TransformKeyFrame b2kf(time, 0);

		animation.getBoneTrack(joints.first)->getInterpolatedKeyFrame(time, &b1kf);
		animation.getBoneTrack(joints.second)->getInterpolatedKeyFrame(time, &b2kf);

		// NOTE: To see positions vs orientations, set jointXTranslation to be bXkf.getTranslation()
		// this orients bones based on absolute positions instead of world coordinates
		// that are calculated using the bone hierarchy and fixed bone lengths
		const glm::mat4 j1(world_transforms[joints.first]);
		const glm::mat4 j2(world_transforms[joints.second]);
		const glm::vec3 joint1Translation(j1[3][0], j1[3][1], j1[3][2]);
		const glm::vec3 joint2Translation(j2[3][0], j2[3][1], j2[3][2]);

		// Get the two joints for this bone
		const Bone bone1(joints.first, getParentBoneID(joints.first), joint1Translation, b1kf.getRotation(), glm::vec3(1));
		const Bone bone2(joints.first, getParentBoneID(joints.first), joint2Translation, b2kf.getRotation(), glm::vec3(1));

		if (bone1.translation != glm::vec3(0) && bone2.translation != glm::vec3(0)) {
			// Calculate orientation and position for cylinder connecting bone1 and bone2
			const float dist        = glm::distance(bone1.translation, bone2.translation);
			const glm::vec3 diff    = bone2.translation - bone1.translation;
			const glm::vec3 forward = glm::normalize(diff);
			const glm::vec3 axis    = glm::cross(glm::vec3(0,1,0), forward);
			const float angle       = glm::degrees(acos(glm::dot(glm::vec3(0,1,0), forward)));

			// Calculate the model matrix for this cylinder using the orientation and position
			model = glm::rotate(glm::translate(glm::mat4(), bone1.translation), angle, axis);
			model = glm::scale(model, glm::vec3(s,dist,s));

			GLUtils::defaultProgram->setUniform("model", model);
			GLUtils::defaultProgram->setUniform("useLighting", 1);
			GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1));
			GLUtils::defaultProgram->setUniform("tex", 0);
			Render::cylinder();
		}
	});
}


void renderAnimation(const Animation& animation, const float time)
{
	using glm::mat4;
	using glm::vec3;

	const vec3 world_scale_factor(0.1f);
	const vec3 joint_scale_factor(0.3f);
	const vec3 axes_scale_factor(1.f);

	if (!offsets_calculated) {
		recalculateBoneOffsets(animation, time);
		offsets_calculated = true;
	}

	TransformKeyFrame kf(time, 0);
	animation.getBoneTrack(HIP_CENTER)->getInterpolatedKeyFrame(time, &kf);

	// Calculate root world transformation
	world_transforms[HIP_CENTER] = glm::translate( mat4(), kf.getTranslation() );
	world_transforms[HIP_CENTER] = glm::scale( world_transforms[HIP_CENTER], world_scale_factor );

	// Calculate world transformation for each joint
	// Note: id ordering ensures parents are calculated before their children 
	for (int boneID = HIP_CENTER; boneID < COUNT; ++boneID) {
		animation.getBoneTrack(boneID)->getInterpolatedKeyFrame(time, &kf);

		const EBoneID parentID = getParentBoneID((EBoneID) boneID);
		const vec3 boneLength(0, glm::length(bone_offsets[boneID]), 0);
		const mat4 rotation( glm::mat4_cast(kf.getRotation()) );

		glm::mat4& boneTransform = world_transforms[boneID];
		boneTransform = world_transforms[parentID];
		boneTransform = boneTransform * rotation;
		boneTransform = glm::translate( boneTransform, boneLength );

		renderJoint( glm::scale( boneTransform, joint_scale_factor ) );
		renderOrientation( glm::scale( boneTransform, axes_scale_factor ) );
	}
	renderBones(animation, time);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------



void exportHeirarchyAsBVH(const Animation *animation, ofstream& fout, zh::EulerRotOrder eulerOrder);
void exportMotionAsBVH(const Animation *animation, ofstream& fout, zh::EulerRotOrder eulerOrder);

std::string eulerOrderFileString(zh::EulerRotOrder eulerOrder)
{
	switch (eulerOrder)
	{
		case zh::EulerRotOrder::EulerRotOrder_XYZ: return "xyz";
		case zh::EulerRotOrder::EulerRotOrder_XZY: return "xzy";
		case zh::EulerRotOrder::EulerRotOrder_YXZ: return "yxz";
		case zh::EulerRotOrder::EulerRotOrder_YZX: return "yzx";
		case zh::EulerRotOrder::EulerRotOrder_ZXY: return "zxy";
		case zh::EulerRotOrder::EulerRotOrder_ZYX: return "zyx";
	}
}

std::string eulerOrderBVHString(zh::EulerRotOrder eulerOrder)
{
	switch (eulerOrder)
	{
		case zh::EulerRotOrder::EulerRotOrder_XYZ: return "Xrotation Yrotation Zrotation";
		case zh::EulerRotOrder::EulerRotOrder_XZY: return "Xrotation Zrotation Yrotation";
		case zh::EulerRotOrder::EulerRotOrder_YXZ: return "Yrotation Xrotation Zrotation";
		case zh::EulerRotOrder::EulerRotOrder_YZX: return "Yrotation Zrotation Xrotation";
		case zh::EulerRotOrder::EulerRotOrder_ZXY: return "Zrotation Xrotation Yrotation";
		case zh::EulerRotOrder::EulerRotOrder_ZYX: return "Zrotation Yrotation Xrotation";
	}
}

void outputAngles(ofstream& fout, const glm::vec3& angles, zh::EulerRotOrder eulerOrder)
{
	switch (eulerOrder)
	{
		case zh::EulerRotOrder::EulerRotOrder_XYZ:
			fout << glm::degrees(angles.x) << " " << glm::degrees(angles.y) << " " << glm::degrees(angles.z) << " ";
			break;
		case zh::EulerRotOrder::EulerRotOrder_XZY:
			fout << glm::degrees(angles.x) << " " << glm::degrees(angles.z) << " " << glm::degrees(angles.y) << " ";
			break;
		case zh::EulerRotOrder::EulerRotOrder_YXZ:
			fout << glm::degrees(angles.y) << " " << glm::degrees(angles.x) << " " << glm::degrees(angles.z) << " ";
			break;
		case zh::EulerRotOrder::EulerRotOrder_YZX:
			fout << glm::degrees(angles.y) << " " << glm::degrees(angles.z) << " " << glm::degrees(angles.x) << " ";
			break;
		case zh::EulerRotOrder::EulerRotOrder_ZXY:
			fout << glm::degrees(angles.z) << " " << glm::degrees(angles.x) << " " << glm::degrees(angles.y) << " ";
			break;
		case zh::EulerRotOrder::EulerRotOrder_ZYX:
			fout << glm::degrees(angles.z) << " " << glm::degrees(angles.y) << " " << glm::degrees(angles.x) << " ";
			break;
	}
}

void exportAnimationAsBVH_WithOrdering(const Animation& animation, zh::EulerRotOrder eulerOrder)
{
	const string filename = animation.getName() + "_" + eulerOrderFileString(eulerOrder) + ".bvh";
	ofstream fout(filename);
	if (!fout.is_open()) {
		cout << "Unable to open file '" << filename << "' for writing.\n";
		return;
	}

	exportHeirarchyAsBVH(&animation, fout, eulerOrder);
	exportMotionAsBVH(&animation, fout, eulerOrder);

	fout.close();
}

void exportAnimationAsBVH(const Animation *animation)
{
	if (nullptr == animation) return;

	exportAnimationAsBVH_WithOrdering(*animation, zh::EulerRotOrder::EulerRotOrder_XYZ);
	// Uncomment for debugging purposes...
	//exportAnimationAsBVH_WithOrdering(*animation, zh::EulerRotOrder::EulerRotOrder_XZY);
	//exportAnimationAsBVH_WithOrdering(*animation, zh::EulerRotOrder::EulerRotOrder_YXZ);
	//exportAnimationAsBVH_WithOrdering(*animation, zh::EulerRotOrder::EulerRotOrder_YZX);
	//exportAnimationAsBVH_WithOrdering(*animation, zh::EulerRotOrder::EulerRotOrder_ZXY);
	//exportAnimationAsBVH_WithOrdering(*animation, zh::EulerRotOrder::EulerRotOrder_ZYX);
}


void exportHeirarchyAsBVH(const Animation *animation, ofstream& fout, zh::EulerRotOrder eulerOrder)
{
	const string translationOrder("Xposition Yposition Zposition");
	const string rotationOrder = eulerOrderBVHString(eulerOrder);
	const float scale = 100.f;

	const auto& boneTracks = animation->getBoneTracks();
	const auto& rootTrack = boneTracks.at(HIP_CENTER);
	const auto& rootKeyFrame = static_cast<TransformKeyFrame*>(rootTrack->getKeyFrame(0));

	vector<glm::vec3> offsets;

	//for (const auto& track : boneTracks) {
	for (unsigned short i = 0; i < EBoneID::COUNT; ++i) {
		const auto& track = boneTracks.at(i);
		const auto& keyFrame = static_cast<TransformKeyFrame*>(track->getKeyFrame(0));
		offsets.push_back(scale * (keyFrame->getTranslation() - rootKeyFrame->getTranslation()));
	}

	// -------------------------------------------------------------------------
	// ROOT
	// -------------------------------------------------------------------------
	fout << "HIERARCHY" << endl;
	fout << "ROOT Hip" << endl;
	fout << "{" << endl;
	fout << "\tOFFSET 0.0 0.0 0.0" << endl;
	fout << "\tCHANNELS 6 " << translationOrder << " " << rotationOrder << endl;

	// -------------------------------------------------------------------------
	// Spine
	fout << "\tJOINT Spine" << endl;
	fout << "\t{" << endl;
	fout << "\t\tOFFSET " << offsets[1].x - offsets[0].x << " " << offsets[1].y - offsets[0].y << " " << offsets[1].z - offsets[0].z << endl;
	fout << "\t\tCHANNELS 3 " << rotationOrder << endl;

	// -------------------------------------------------------------------------
	// Shoulder Center
	fout << "\t\tJOINT ShoulderCenter" << endl;
	fout << "\t\t{" << endl;
	fout << "\t\t\tOFFSET " << offsets[2].x - offsets[1].x << " " << offsets[2].y - offsets[1].y << " " << offsets[2].z - offsets[1].z << endl;
	fout << "\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Head
	fout << "\t\t\tJOINT Head" << endl;
	fout << "\t\t\t{" << endl;
	fout << "\t\t\t\tOFFSET " << offsets[3].x - offsets[2].x << " " << offsets[3].y - offsets[2].y << " " << offsets[3].z - offsets[2].z << endl;
	fout << "\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// End Site
	fout << "\t\t\t\tEnd Site" << endl;
	fout << "\t\t\t\t{" << endl;
	fout << "\t\t\t\t\tOFFSET 0.0 0.0 0.0" << endl;
	fout << "\t\t\t\t}" << endl;
	fout << "\t\t\t}" << endl;

	// -------------------------------------------------------------------------
	// Shoulder Left
	fout << "\t\t\tJOINT ShoulderLeft" << endl;
	fout << "\t\t\t{" << endl;
	fout << "\t\t\t\tOFFSET " << offsets[4].x - offsets[2].x << " " << offsets[4].y - offsets[2].y << " " << offsets[4].z - offsets[2].z << endl;
	fout << "\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Elbow Left
	fout << "\t\t\t\tJOINT ElbowLeft" << endl;
	fout << "\t\t\t\t{" << endl;
	fout << "\t\t\t\t\tOFFSET " << offsets[5].x - offsets[4].x << " " << offsets[5].y - offsets[4].y << " " << offsets[5].z - offsets[4].z << endl;
	fout << "\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Wrist Left
	fout << "\t\t\t\t\tJOINT WristLeft" << endl;
	fout << "\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\tOFFSET " << offsets[6].x - offsets[5].x << " " << offsets[6].y - offsets[5].y << " " << offsets[6].z - offsets[5].z << endl;
	fout << "\t\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Hand Left
	fout << "\t\t\t\t\t\tJOINT HandLeft" << endl;
	fout << "\t\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\t\tOFFSET " << offsets[7].x - offsets[6].x << " " << offsets[7].y - offsets[6].y << " " << offsets[7].z - offsets[6].z << endl;
	fout << "\t\t\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// End Site
	fout << "\t\t\t\t\t\t\tEnd Site" << endl;
	fout << "\t\t\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << endl;
	fout << "\t\t\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t}" << endl;
	fout << "\t\t\t}" << endl;

	// -------------------------------------------------------------------------
	// Shoulder Right
	fout << "\t\t\tJOINT ShoulderRight" << endl;
	fout << "\t\t\t{" << endl;
	fout << "\t\t\t\tOFFSET " << offsets[8].x - offsets[2].x << " " << offsets[8].y - offsets[2].y << " " << offsets[8].z - offsets[2].z << endl;
	fout << "\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Elbow Right
	fout << "\t\t\t\tJOINT ElbowRight" << endl;
	fout << "\t\t\t\t{" << endl;
	fout << "\t\t\t\t\tOFFSET " << offsets[9].x - offsets[8].x << " " << offsets[9].y - offsets[8].y << " " << offsets[9].z - offsets[8].z << endl;
	fout << "\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Wrist Right
	fout << "\t\t\t\t\tJOINT WristRight" << endl;
	fout << "\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\tOFFSET " << offsets[10].x - offsets[9].x << " " << offsets[10].y - offsets[9].y << " " << offsets[10].z - offsets[9].z << endl;
	fout << "\t\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Hand Right
	fout << "\t\t\t\t\t\tJOINT HandRight" << endl;
	fout << "\t\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\t\tOFFSET " << offsets[11].x - offsets[10].x << " " << offsets[11].y - offsets[10].y << " " << offsets[11].z - offsets[10].z << endl;
	fout << "\t\t\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// End Site
	fout << "\t\t\t\t\t\t\tEnd Site" << endl;
	fout << "\t\t\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << endl;
	fout << "\t\t\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t}" << endl;
	fout << "\t\t\t}" << endl;

	fout << "\t\t}" << endl;

	fout << "\t}" << endl;

	// -------------------------------------------------------------------------
	// Hip Left
	fout << "\tJOINT HipLeft" << endl;
	fout << "\t{" << endl;
	fout << "\t\tOFFSET " << offsets[12].x - offsets[0].x << " " << offsets[12].y - offsets[0].y << " " << offsets[12].z - offsets[0].z << endl;
	fout << "\t\tCHANNELS 3 " << rotationOrder << endl;
	// Knee Left
	fout << "\t\tJOINT KneeLeft" << endl;
	fout << "\t\t{" << endl;
	fout << "\t\t\tOFFSET " << offsets[13].x - offsets[12].x << " " << offsets[13].y - offsets[12].y << " " << offsets[13].z - offsets[12].z << endl;
	fout << "\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Ankle Left
	fout << "\t\t\tJOINT AnkleLeft" << endl;
	fout << "\t\t\t{" << endl;
	fout << "\t\t\t\tOFFSET " << offsets[14].x - offsets[13].x << " " << offsets[14].y - offsets[13].y << " " << offsets[14].z - offsets[13].z << endl;
	fout << "\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Foot Left
	fout << "\t\t\t\tJOINT FootLeft" << endl;
	fout << "\t\t\t\t{" << endl;
	fout << "\t\t\t\t\tOFFSET " << offsets[15].x - offsets[14].x << " " << offsets[15].y - offsets[14].y << " " << offsets[15].z - offsets[14].z << endl;
	fout << "\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// End Site
	fout << "\t\t\t\t\tEnd Site" << endl;
	fout << "\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << endl;
	fout << "\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t}" << endl;
	fout << "\t\t\t}" << endl;
	fout << "\t\t}" << endl;
	fout << "\t}" << endl;

	// -------------------------------------------------------------------------
	// Hip Right
	fout << "\tJOINT HipRight" << endl;
	fout << "\t{" << endl;
	fout << "\t\tOFFSET " << offsets[16].x - offsets[0].x << " " << offsets[16].y - offsets[0].y << " " << offsets[16].z - offsets[0].z << endl;
	fout << "\t\tCHANNELS 3 " << rotationOrder << endl;
	// Knee Right
	fout << "\t\tJOINT KneeRight" << endl;
	fout << "\t\t{" << endl;
	fout << "\t\t\tOFFSET " << offsets[17].x - offsets[16].x << " " << offsets[17].y - offsets[16].y << " " << offsets[17].z - offsets[16].z << endl;
	fout << "\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Ankle Right
	fout << "\t\t\tJOINT AnkleRight" << endl;
	fout << "\t\t\t{" << endl;
	fout << "\t\t\t\tOFFSET " << offsets[18].x - offsets[17].x << " " << offsets[18].y - offsets[17].y << " " << offsets[18].z - offsets[17].z << endl;
	fout << "\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// Foot Right
	fout << "\t\t\t\tJOINT FootRight" << endl;
	fout << "\t\t\t\t{" << endl;
	fout << "\t\t\t\t\tOFFSET " << offsets[19].x - offsets[18].x << " " << offsets[19].y - offsets[18].y << " " << offsets[19].z - offsets[18].z << endl;
	fout << "\t\t\t\t\tCHANNELS 3 " << rotationOrder << endl;
	// End Site
	fout << "\t\t\t\t\tEnd Site" << endl;
	fout << "\t\t\t\t\t{" << endl;
	fout << "\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << endl;
	fout << "\t\t\t\t\t}" << endl;
	fout << "\t\t\t\t}" << endl;
	fout << "\t\t\t}" << endl;
	fout << "\t\t}" << endl;
	fout << "\t}" << endl;

	fout << "}" << endl;
}

void outputRotationAngles(int boneId, int i, const Animation *animation, ofstream& fout, zh::EulerRotOrder eulerOrder)
{
	const auto& track = animation->getBoneTrack(boneId);
	const auto& keyFrame = static_cast<TransformKeyFrame*>(track->getKeyFrame(i));
	const glm::quat& q(keyFrame->getRotation());
	const zh::Quat rotation(q.w, q.x, q.y, q.z);

	glm::vec3 angles;
	rotation.getEuler(angles.x, angles.y, angles.z, eulerOrder);

	outputAngles(fout, angles, eulerOrder);
}

void exportMotionAsBVH(const Animation *animation, ofstream& fout, zh::EulerRotOrder eulerOrder)
{
	const float translation_scale = 100.f;
	const auto& rootTrack = animation->getBoneTrack(HIP_CENTER);
	const int numFrames = rootTrack->getNumKeyFrames();

	fout << "\nMOTION" << endl;
	fout << "Frames: " << numFrames << endl;
	fout << "Frame Time: " << "0.0333333" << endl;

	for (int i = 0; i < numFrames; ++i) {
		const auto& rootKeyFrame = static_cast<TransformKeyFrame*>(rootTrack->getKeyFrame(i));
		const glm::vec3 pos = rootKeyFrame->getTranslation() * translation_scale;

		fout << pos.x << " " << pos.y << " " << pos.z << " ";
		for (int boneId = 0; boneId < EBoneID::COUNT; ++boneId) {
			outputRotationAngles(boneId, i, animation, fout, eulerOrder);
		}
		fout << endl;
	}
}
