#include "AnimationTypes.h"


const BoneMask empty_bone_mask = BoneMask();

const BoneMask default_bone_mask = ([]() -> BoneMask {
	BoneMask mask;
	for (int id = 0; id < EBoneID::COUNT; ++id) {
		mask.insert((EBoneID) id);
	}
	return mask;
} ());

const BoneMask seated_bone_mask = ([]() -> BoneMask {
	BoneMask mask;
	for (int id = SHOULDER_CENTER; id <= HAND_RIGHT; ++id) {
		mask.insert((EBoneID) id);
	}
	return mask;
} ());
