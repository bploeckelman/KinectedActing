#include "Animation.h"
#include "BoneAnimationTrack.h"
#include "TransformKeyFrame.h"

#include "Skeleton.h"

#include <algorithm>

bool hasBoneTrack( const Animation& animation, unsigned short boneId )
{
	return animation.getBoneTracks().find(boneId) != end(animation.getBoneTracks());
}

Animation::Animation( unsigned short id, const std::string& name )
	: mId(id)
	, mName(name)
	, mInterpMethod(KFInterp_Linear)
{}

Animation::~Animation()
{
	deleteAllBoneTrack();
}

void Animation::apply( Skeleton* skel, float time, float weight/*=1.f*/, float scale/*=1.f*/, const BoneMask& boneMask/*=default_bone_mask*/ ) const
{
	assert(nullptr != skel);

	// apply bone tracks
	std::for_each(begin(boneMask), end(boneMask), [&](const EBoneID& boneID) {
		const BoneAnimationTrack& bt = *mBoneTracks.at(boneID);
		bt.apply(skel, time, weight, scale);
	});
}

void Animation::getPositions( unsigned short boneId, std::vector<glm::vec3>& positions, float lastTime/*=-1.f*/ ) const
{
	const auto& track = mBoneTracks.at(boneId);
	positions.clear();
	positions.resize(track->getNumKeyFrames());

	unsigned int i = 0;
	for (const auto& keyframe : track->getKeyFrames()) {
		if (lastTime == -1.f || keyframe->getTime() < lastTime) {
			positions[i++] = static_cast<const TransformKeyFrame*>(keyframe)->getTranslation();
		}
	}
}

BoneAnimationTrack* Animation::createBoneTrack( unsigned short boneId )
{
	BoneAnimationTrack* bat = new BoneAnimationTrack(boneId, this);
	mBoneTracks.insert(std::make_pair(bat->getBoneId(), bat));
	return bat;
}

void Animation::deleteBoneTrack( unsigned short boneId )
{
	BoneTrackIterator bti = mBoneTracks.find(boneId);
	if( bti != end(mBoneTracks)  )
	{
		delete bti->second;
		mBoneTracks.erase(bti);
	}
}

void Animation::deleteAllBoneTrack()
{
	for(BoneTrackIterator bti = begin(mBoneTracks); bti != end(mBoneTracks); ++bti)
	{
		delete bti->second;
	}

	mBoneTracks.clear();
}

BoneAnimationTrack* Animation::getBoneTrack(unsigned short boneId) const
{
	return (hasBoneTrack(*this, boneId) ? mBoneTracks.at(boneId) : nullptr);
}

void Animation::setKFInterpMethod(KFInterpMethod interpMethod)
{
	mInterpMethod = interpMethod;
	if( mInterpMethod == KFInterp_Spline )
	{
		for(BoneTrackIterator bti = begin(mBoneTracks); bti != end(mBoneTracks); ++bti)
		{
			BoneAnimationTrack& bt = *bti->second;
			bt._buildInterpSplines();
		}
	}

}

float Animation::getLength() const
{
	float cur_length = 0, length = 0;

	for(BoneTrackConstIterator bti = begin(mBoneTracks); bti != end(mBoneTracks); ++bti)
	{
		const BoneAnimationTrack& bt = *bti->second;
		if( (cur_length = bt.getLength() ) > length )
			length = cur_length;
	}

	return length;
}

size_t Animation::_calcMemoryUsage() const
{
	size_t mem_usage = 0;

	// Compute memory usage from animation tracks
	for( BoneTrackConstIterator bti = begin(mBoneTracks); bti != end(mBoneTracks); ++bti)
	{
		const BoneAnimationTrack& boneTrack = *bti->second;
		mem_usage += boneTrack.getNumKeyFrames()
		          * (sizeof(float) + 2 * sizeof(glm::vec3) + sizeof(glm::quat));
	}

	return mem_usage;
}
