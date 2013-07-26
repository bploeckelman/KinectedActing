#pragma once

#include <map>
#include <set>


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

typedef std::set<EBoneID> BoneMask;

extern const BoneMask empty_bone_mask;
extern const BoneMask default_bone_mask;
extern const BoneMask seated_bone_mask;
