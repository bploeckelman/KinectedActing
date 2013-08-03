#include "Recording.h"
#include "Skeleton.h"
#include "Animation.h"
#include "AnimationTypes.h"
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

void Recording::apply( Skeleton *skeleton, float time, const BoneMask& boneMask/*=default_bone_mask*/ )
{
	if (getAnimationLength() > 0.f) {
		animation->apply(skeleton, time, 1.f, 1.f, boneMask);
	}
}

void Recording::apply( Skeleton *skeleton, const BoneMask& boneMask/*=default_bone_mask*/ )
{
	if (getAnimationLength() > 0.f) {
		animation->apply(skeleton, playbackTime, 1.f, 1.f, boneMask);
	}
}

void Recording::updateRecording( float delta )
{
	if (!recording) return;

	// Update animation timer for this new keyframe
	recordingTime += recordingDelta; //delta;
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
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage(
		  "Saved " + std::to_string(numKeyFrames) + " key frames\n"
		+ "Mem usage: " + std::to_string(animation->_calcMemoryUsage()) + " bytes\n" ));
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
	const auto& skeletonFrame    = kinect.getSkeletonFrame();
	const auto *skeletonData     = kinect.getFirstTrackedSkeletonData(skeletonFrame);
	const auto *boneOrientations = kinect.getOrientations();
	if (nullptr == skeletonData) return 0;

	// Update all bone tracks with a new keyframe
	BoneAnimationTrack *track   = nullptr;
	TransformKeyFrame *keyFrame = nullptr;
	size_t numKeyFrames = 0;
	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		track = animation->getBoneTrack(boneID);
		if (nullptr == track) continue;

		keyFrame = static_cast<TransformKeyFrame*>(track->createKeyFrame(now));
		if (nullptr == keyFrame) continue;

		const Vector4& p = skeletonData->SkeletonPositions[boneID];
		// TODO : save both absolute rotation and hierarchical rotation
		const Vector4& q = boneOrientations[boneID].absoluteRotation.rotationQuaternion;
		keyFrame->setTranslation(glm::vec3(p.x, p.y, p.z));
		keyFrame->setRotation(glm::quat(q.w, q.x, q.y, q.z));
		keyFrame->setScale(glm::vec3(1,1,1));

		numKeyFrames += track->getNumKeyFrames();
	}

	return numKeyFrames;
}

void Recording::clearRecording()
{
	animation->deleteAllBoneTrack();

	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		animation->createBoneTrack(boneID);
	}

	playback  = false;
	recording = false;

	playbackTime  = 0.f;
	recordingTime = 0.f;
}

void Recording::setPlaybackTime( float t )
{
	playbackTime = glm::clamp<float>(t, 0.f, animation->getLength());
}

void Recording::playbackNextFrame() {
	const float length = animation->getLength();
	playbackTime += playbackDelta;
	if (playbackTime > length) {
		playbackTime = length;
	}
}

void Recording::playbackPreviousFrame() {
	playbackTime -= playbackDelta;
	if (playbackTime < 0.f) {
		playbackTime = 0.f;
	}
}

float Recording::getAnimationLength() const { return ((nullptr == animation) ? 0.f : animation->getLength()); }
