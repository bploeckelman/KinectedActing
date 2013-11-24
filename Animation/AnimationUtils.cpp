#include "AnimationUtils.h"

#include "Skeleton.h"
#include "Animation.h"
#include "TransformKeyFrame.h"
#include "BoneAnimationTrack.h"

#include <iostream>
#include <fstream>
#include <vector>

void exportHeirarchyAsBVH(const Animation *animation, std::ofstream& fout);
void exportMotionAsBVH(const Animation *animation, std::ofstream& fout);


void exportAnimationAsBVH(const Animation *animation)
{
	if (nullptr == animation) return;

	const std::string filename = animation->getName() + ".bvh";
	std::ofstream fout(filename);
	if (!fout.is_open()) {
		std::cout << "Unable to open file '" << filename << "' for writing.\n";
		return;
	}

	exportHeirarchyAsBVH(animation, fout);
	exportMotionAsBVH(animation, fout);

	fout.close();
}


void exportHeirarchyAsBVH(const Animation *animation, std::ofstream& fout)
{
	const auto& boneTracks = animation->getBoneTracks();
	const auto& rootTrack = boneTracks.at(HIP_CENTER);
	const auto& rootKeyFrame = static_cast<TransformKeyFrame*>(rootTrack->getKeyFrame(0));

	std::vector<glm::vec3> offsets;

	for (const auto& track : boneTracks) {
		const auto& keyFrame = static_cast<TransformKeyFrame*>(track.second->getKeyFrame(0));
		offsets.push_back(keyFrame->getTranslation() - rootKeyFrame->getTranslation());
	}

	// -------------------------------------------------------------------------
	// ROOT
	// -------------------------------------------------------------------------
	fout << "HIERARCHY" << std::endl;
	fout << "ROOT Hip" << std::endl;
	fout << "{" << std::endl;

	// -------------------------------------------------------------------------
	// Spine
	fout << "\tOFFSET " << offsets[0].x << " " << offsets[0].y << " " << offsets[0].z << std::endl;
	fout << "\tCHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation" << std::endl;
	fout << "\tJOINT Spine" << std::endl;
	fout << "\t{" << std::endl;

	// -------------------------------------------------------------------------
	// Shoulder Center
	fout << "\t\tOFFSET " << offsets[1].x - offsets[0].x << " " << offsets[1].y - offsets[0].y << " " << offsets[1].z - offsets[0].z << std::endl;
	fout << "\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\tJOINT ShoulderCenter" << std::endl;
	fout << "\t\t{" << std::endl;
	// Head
	fout << "\t\t\tOFFSET " << offsets[2].x - offsets[1].x << " " << offsets[2].y - offsets[1].y << " " << offsets[2].z - offsets[1].z << std::endl;
	fout << "\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\tJOINT Head" << std::endl;
	fout << "\t\t\t{" << std::endl;
	// End Site
	fout << "\t\t\t\tOFFSET " << offsets[3].x - offsets[2].x << " " << offsets[3].y - offsets[2].y << " " << offsets[3].z - offsets[2].z << std::endl;
	fout << "\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\tEnd Site" << std::endl;
	fout << "\t\t\t\t{" << std::endl;
	fout << "\t\t\t\t\tOFFSET 0.0 0.0 0.0" << std::endl;
	fout << "\t\t\t\t}" << std::endl;
	fout << "\t\t\t}" << std::endl;

	// -------------------------------------------------------------------------
	// Shoulder Left
	fout << "\t\t\tJOINT ShoulderLeft" << std::endl;
	fout << "\t\t\t{" << std::endl;
	// Elbow Left
	fout << "\t\t\t\tOFFSET " << offsets[4].x - offsets[2].x << " " << offsets[4].y - offsets[2].y << " " << offsets[4].z - offsets[2].z << std::endl;
	fout << "\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\tJOINT ElbowLeft" << std::endl;
	fout << "\t\t\t\t{" << std::endl;
	// Wrist Left
	fout << "\t\t\t\t\tOFFSET " << offsets[5].x - offsets[4].x << " " << offsets[5].y - offsets[4].y << " " << offsets[5].z - offsets[4].z << std::endl;
	fout << "\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\tJOINT WristLeft" << std::endl;
	fout << "\t\t\t\t\t{" << std::endl;
	// Hand Left
	fout << "\t\t\t\t\t\tOFFSET " << offsets[6].x - offsets[5].x << " " << offsets[6].y - offsets[5].y << " " << offsets[6].z - offsets[5].z << std::endl;
	fout << "\t\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\t\tJOINT HandLeft" << std::endl;
	fout << "\t\t\t\t\t\t{" << std::endl;
	// End Site
	fout << "\t\t\t\t\t\t\tOFFSET " << offsets[7].x - offsets[6].x << " " << offsets[7].y - offsets[6].y << " " << offsets[7].z - offsets[6].z << std::endl;
	fout << "\t\t\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\t\t\tEnd Site" << std::endl;
	fout << "\t\t\t\t\t\t\t{" << std::endl;
	fout << "\t\t\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << std::endl;
	fout << "\t\t\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t}" << std::endl;
	fout << "\t\t\t}" << std::endl;

	// -------------------------------------------------------------------------
	// Shoulder Right
	fout << "\t\t\tJOINT ShoulderRight" << std::endl;
	fout << "\t\t\t{" << std::endl;
	// Elbow Right
	fout << "\t\t\t\tOFFSET " << offsets[8].x - offsets[2].x << " " << offsets[8].y - offsets[2].y << " " << offsets[8].z - offsets[2].z << std::endl;
	fout << "\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\tJOINT ElbowRight" << std::endl;
	fout << "\t\t\t\t{" << std::endl;
	// Wrist Right
	fout << "\t\t\t\t\tOFFSET " << offsets[9].x - offsets[8].x << " " << offsets[9].y - offsets[8].y << " " << offsets[9].z - offsets[8].z << std::endl;
	fout << "\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\tJOINT WristRight" << std::endl;
	fout << "\t\t\t\t\t{" << std::endl;
	// Hand Right
	fout << "\t\t\t\t\t\tOFFSET " << offsets[10].x - offsets[9].x << " " << offsets[10].y - offsets[9].y << " " << offsets[10].z - offsets[9].z << std::endl;
	fout << "\t\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\t\tJOINT HandRight" << std::endl;
	fout << "\t\t\t\t\t\t{" << std::endl;
	// End Site
	fout << "\t\t\t\t\t\t\tOFFSET " << offsets[11].x - offsets[10].x << " " << offsets[11].y - offsets[10].y << " " << offsets[11].z - offsets[10].z << std::endl;
	fout << "\t\t\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\t\t\tEnd Site" << std::endl;
	fout << "\t\t\t\t\t\t\t{" << std::endl;
	fout << "\t\t\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << std::endl;
	fout << "\t\t\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t}" << std::endl;
	fout << "\t\t\t}" << std::endl;

	fout << "\t\t}" << std::endl;

	fout << "\t}" << std::endl;

	// -------------------------------------------------------------------------
	// Hip Left
	fout << "\tJOINT HipLeft" << std::endl;
	fout << "\t{" << std::endl;
	// Knee Left
	fout << "\t\tOFFSET " << offsets[12].x - offsets[0].x << " " << offsets[12].y - offsets[0].y << " " << offsets[12].z - offsets[0].z << std::endl;
	fout << "\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\tJOINT KneeLeft" << std::endl;
	fout << "\t\t{" << std::endl;
	// Ankle Left
	fout << "\t\t\tOFFSET " << offsets[13].x - offsets[12].x << " " << offsets[13].y - offsets[12].y << " " << offsets[13].z - offsets[12].z << std::endl;
	fout << "\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\tJOINT AnkleLeft" << std::endl;
	fout << "\t\t\t{" << std::endl;
	// Foot Left
	fout << "\t\t\t\tOFFSET " << offsets[14].x - offsets[13].x << " " << offsets[14].y - offsets[13].y << " " << offsets[14].z - offsets[13].z << std::endl;
	fout << "\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\tJOINT FootLeft" << std::endl;
	fout << "\t\t\t\t{" << std::endl;
	// End Site
	fout << "\t\t\t\t\tOFFSET " << offsets[15].x - offsets[14].x << " " << offsets[15].y - offsets[14].y << " " << offsets[15].z - offsets[14].z << std::endl;
	fout << "\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\tEnd Site" << std::endl;
	fout << "\t\t\t\t\t{" << std::endl;
	fout << "\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << std::endl;
	fout << "\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t}" << std::endl;
	fout << "\t\t\t}" << std::endl;
	fout << "\t\t}" << std::endl;
	fout << "\t}" << std::endl;

	// -------------------------------------------------------------------------
	// Hip Right
	fout << "\tJOINT HipRight" << std::endl;
	fout << "\t{" << std::endl;
	// Knee Right
	fout << "\t\tOFFSET " << offsets[16].x - offsets[0].x << " " << offsets[16].y - offsets[0].y << " " << offsets[16].z - offsets[0].z << std::endl;
	fout << "\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\tJOINT KneeRight" << std::endl;
	fout << "\t\t{" << std::endl;
	// Ankle Right
	fout << "\t\t\tOFFSET " << offsets[17].x - offsets[16].x << " " << offsets[17].y - offsets[16].y << " " << offsets[17].z - offsets[16].z << std::endl;
	fout << "\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\tJOINT AnkleRight" << std::endl;
	fout << "\t\t\t{" << std::endl;
	// Foot Right
	fout << "\t\t\t\tOFFSET " << offsets[18].x - offsets[17].x << " " << offsets[18].y - offsets[17].y << " " << offsets[18].z - offsets[17].z << std::endl;
	fout << "\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\tJOINT FootRight" << std::endl;
	fout << "\t\t\t\t{" << std::endl;
	// End Site
	fout << "\t\t\t\t\tOFFSET " << offsets[19].x - offsets[18].x << " " << offsets[19].y - offsets[18].y << " " << offsets[19].z - offsets[18].z << std::endl;
	fout << "\t\t\t\t\tCHANNELS 3 Xrotation Yrotation Zrotation" << std::endl;
	fout << "\t\t\t\t\tEnd Site" << std::endl;
	fout << "\t\t\t\t\t{" << std::endl;
	fout << "\t\t\t\t\t\tOFFSET 0.0 0.0 0.0" << std::endl;
	fout << "\t\t\t\t\t}" << std::endl;
	fout << "\t\t\t\t}" << std::endl;
	fout << "\t\t\t}" << std::endl;
	fout << "\t\t}" << std::endl;
	fout << "\t}" << std::endl;

	fout << "}" << std::endl;
}


void exportMotionAsBVH(const Animation *animation, std::ofstream& fout)
{
	// while HIP_CENTER is root of the bvh hierarchy, SHOULDER_CENTER is recorded in both seated and non-seated mode
	const auto& rootTrack = animation->getBoneTrack(HIP_CENTER);
	const int numFrames = rootTrack->getNumKeyFrames();

	fout << "\nMOTION" << std::endl;
	fout << "Frames: " << numFrames << std::endl;
	fout << "Frame Time: " << '60' << std::endl;

	for (int i = 0; i < numFrames; ++i) {
		const auto& rootKeyFrame = static_cast<TransformKeyFrame*>(rootTrack->getKeyFrame(i));

		const glm::vec3 pos(rootKeyFrame->getTranslation());
		fout << pos.x << " " << pos.y << " " << pos.z << " ";

		for (int boneId = 0; boneId < EBoneID::COUNT; ++boneId) {
			const auto& track = animation->getBoneTrack(boneId);
			const auto& keyFrame = static_cast<TransformKeyFrame*>(track->getKeyFrame(i));
			const glm::quat& rotation(keyFrame->getRotation());
			const glm::vec3 euler(glm::eulerAngles(rotation));

			fout << euler.x << " " << euler.y << " " << euler.z << " ";
		}

		fout << std::endl;
	}
}
