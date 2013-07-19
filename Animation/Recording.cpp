#include "Recording.h"
#include "Skeleton.h"
#include "TransformKeyFrame.h"
#include "BoneAnimationTrack.h"
#include "Kinect/KinectDevice.h"
#include "Core/Messages/Messages.h"


unsigned int Recording::nextAnimationID = 0;


Recording::Recording( const std::string& name, const KinectDevice& kinect )
	: animation(new Animation(nextAnimationID++, name))
	, kinect(kinect)
	, bonepaths(false)
	, playback(false)
	, playbackTime(0)
	, playbackDelta(1 / 60.f)
	, recording(false)
	, recordingTime(0)
	, recordingDelta(1 / 60.f)
{
	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		animation->createBoneTrack(boneID);
	}
}

Recording::~Recording()
{}

void Recording::update( float delta )
{
	updateRecording(delta);
	updatePlayback(delta);
}

void Recording::apply( Skeleton *skeleton, float time, const std::set<EBoneID>& boneMask/*=EmptyBoneMask*/ )
{
	animation->apply(skeleton, time, 1.f, 1.f, boneMask);
}

void Recording::updateRecording( float delta )
{
	// Can't save if there's nothing to save to
	if (nullptr == animation) return;

	// Update animation timer for this new keyframe
	recordingTime += delta;
	size_t numKeyFrames = saveKeyFrame(recordingTime);

	//if (layering) {
	//	if (now < animLayer["base"]->getLength()) {
	//		saveBlendKeyFrame(now, animLayer["blend"].get());
	//	} else {
	//		msg::StopRecordingMessage msg;
	//		process(&msg);
	//		MessageBoxA(NULL, "Done recording layer", "Done", MB_OK);
	//	}
	//}

	// Update gui label text
	const std::string text = "Saved " + std::to_string(numKeyFrames) + " key frames\n"
		+ "Mem usage: " + std::to_string(animation->_calcMemoryUsage()) + " bytes\n";
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage(text));
}

void Recording::updatePlayback( float delta )
{
	if (!playback) return;

	playbackTime += playbackDelta;

	if (playbackTime > animation->getLength()) {
		playbackTime = 0.f;
	}
}

size_t Recording::saveKeyFrame(float now)
{
	if (nullptr == animation) return 0;

	// Get the Kinect skeleton data if there is any
	const NUI_SKELETON_FRAME& skeletonFrame = kinect.getSkeletonFrame();
	const NUI_SKELETON_DATA *skeletonData = kinect.getFirstTrackedSkeletonData(skeletonFrame);
	const NUI_SKELETON_BONE_ORIENTATION *boneOrientations = kinect.getOrientations();
	if (nullptr == skeletonData) return 0;

	// Update all bone tracks with a new keyframe
	BoneAnimationTrack *track = nullptr;
	TransformKeyFrame *keyFrame = nullptr;
	size_t numKeyFrames = 0;
	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		track = animation->getBoneTrack(boneID);
		if (nullptr == track) continue;

		keyFrame = static_cast<TransformKeyFrame*>(track->createKeyFrame(now));
		if (nullptr == keyFrame) continue;

		const Vector4& p = skeletonData->SkeletonPositions[boneID];
		const Vector4& q = boneOrientations[boneID].absoluteRotation.rotationQuaternion;
		keyFrame->setTranslation(glm::vec3(p.x, p.y, p.z));
		keyFrame->setRotation(glm::quat(q.w, q.x, q.y, q.z));
		keyFrame->setScale(glm::vec3(1,1,1));

		numKeyFrames += track->getNumKeyFrames();
	}

	return numKeyFrames;
}
