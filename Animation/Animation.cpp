#include "Animation.h"
#include "BoneAnimationTrack.h"



Animation::Animation( unsigned short id, const std::string& name /*, AnimationSetPtr animSet*/ )
	: mId(id)
	, mName(name)
	, mFrameRate(60)
	//, mAnimSet(animSet)
	, mInterpMethod(KFInterp_Spline)
{}

Animation::~Animation()
{
	deleteAllBoneTrack();
}

BoneAnimationTrack* Animation::createBoneTrack( unsigned short boneId )
{
	assert(!hasBoneTrack(boneId));

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

bool Animation::hasBoneTrack( unsigned short boneId ) const
{
	return mBoneTracks.find(boneId) != end(mBoneTracks);
}

BoneAnimationTrack* Animation::getBoneTrack(unsigned short boneId) const
{
	if( !hasBoneTrack(boneId) )
		return nullptr;

	return mBoneTracks.at(boneId);
}

Animation::BoneTrackIterator Animation::getBoneTrackIterator()
{
	return begin(mBoneTracks);
}

Animation::BoneTrackConstIterator Animation::getBoneTrackConstIterator() const
{
	return begin(mBoneTracks);
}

void Animation::setKFInterpolationMethod(KFInterpolationMethod interpMethod)
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

void Animation::apply( Skeleton* skel, float time, float weight/*=1.f*/, float scale/*=1.f*/, const std::set<EBoneID> boneMask/*=EmptyBoneMask*/ ) const
{
	assert(nullptr != skel);

	// apply bone tracks
	for(BoneTrackConstIterator bti = begin(mBoneTracks); bti != end(mBoneTracks); ++bti)
	{
		const BoneAnimationTrack& bt = *bti->second;
		bt.apply(skel, time, weight, scale);
	}
}

void Animation::computeAnimationBounds( float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ ) const
{
	// TODO ...
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

void Animation::_clone( Animation* clonePtr ) const
{
	// TODO ...
}
