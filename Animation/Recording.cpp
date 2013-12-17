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
	, looping(true)
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
		const Vector4& q = boneOrientations[boneID].hierarchicalRotation.rotationQuaternion;
		const Vector4& a = boneOrientations[boneID].absoluteRotation.rotationQuaternion;
		keyFrame->setTranslation(glm::vec3(p.x, p.y, p.z));
		keyFrame->setRotation(glm::normalize(glm::quat(q.w, q.x, q.y, q.z)));
		keyFrame->setAbsRotation(glm::normalize(glm::quat(a.w, a.x, a.y, a.z)));
		keyFrame->setScale(glm::vec3(1));

		numKeyFrames += track->getNumKeyFrames();
	}

	return numKeyFrames;
}

void Recording::saveBlendFrame( float time
                              , const Recording& base
                              , const Recording& layer
                              , const BoneMask& boneMask/*=default_bone_mask */
                              , const ELayerMappingMode& mappingMode/*=ELayerMappingMode::MAP_ADDITIVE*/ )
{
	const Animation *baseAnim  = base.getAnimation();
	const Animation *layerAnim = layer.getAnimation();

	BoneAnimationTrack *baseTrack  = nullptr;
	BoneAnimationTrack *layerTrack = nullptr;
	BoneAnimationTrack *blendTrack = nullptr;

	TransformKeyFrame baseKeyFrame(0.f, 0);
	TransformKeyFrame layerKeyFrame1(0.f, 0);
	TransformKeyFrame layerKeyFrame2(0.f, 0);

	// Get position of root bone for base and layer in order to calculate offset of masked bones
	TransformKeyFrame baseRootKeyFrame(0.f, 0);
	TransformKeyFrame layerRootKeyFrame(0.f, 0);
	baseAnim ->getBoneTrack(HIP_CENTER)->getInterpolatedKeyFrame(time, &baseRootKeyFrame);
	layerAnim->getBoneTrack(HIP_CENTER)->getInterpolatedKeyFrame(time, &layerRootKeyFrame);
	const glm::vec3 baseRootPos  = baseRootKeyFrame.getTranslation();
	const glm::vec3 layerRootPos = layerRootKeyFrame.getTranslation();

	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		// Get this bone's animation track for the base, layer, and blend (i.e. this) animations
		baseTrack  = baseAnim->getBoneTrack(boneID);
		layerTrack = layerAnim->getBoneTrack(boneID);
		blendTrack = animation->getBoneTrack(boneID);
		if (nullptr == baseTrack || nullptr == layerTrack || nullptr == blendTrack) continue;

		// Create a new keyframe in this blended recording
		TransformKeyFrame *blendTKF = static_cast<TransformKeyFrame*>(blendTrack->createKeyFrame(time));
		if (nullptr == blendTKF) continue;
		blendTKF->setScale(glm::vec3(1)); // scale is ignored

		// If this boneID is not in boneMask, save the base keyframe for this bone
		if (end(boneMask) == boneMask.find((EBoneID) boneID)) {
			baseTrack->getInterpolatedKeyFrame(time, blendTKF);
			continue;
		}
		// Else this boneID is in boneMask, save a blended keyframe using the current mapping mode...
		switch (mappingMode)
		{
			case MAP_DIRECT:
			{
				layerTrack->getInterpolatedKeyFrame(time, blendTKF);
				blendTKF->setRotation(glm::normalize(blendTKF->getRotation()));
			}
			break;

			case MAP_ABSOLUTE:
			{
				baseTrack->getInterpolatedKeyFrame(0.f, &baseKeyFrame);
				layerTrack->getInterpolatedKeyFrame(time, &layerKeyFrame1);
				layerTrack->getInterpolatedKeyFrame(0.f, &layerKeyFrame2);

				const glm::vec3 boneOffsetPos = layerKeyFrame1.getTranslation() - layerRootPos;

				const glm::vec3 y0(baseKeyFrame.getTranslation());
				const glm::vec3 xt(baseRootPos + boneOffsetPos);
				const glm::vec3 x0(layerKeyFrame2.getTranslation());

				const glm::quat yr0 = baseKeyFrame.getRotation();
				const glm::quat xrt = layerKeyFrame1.getRotation();
				const glm::quat inv_xr0 = glm::inverse(layerKeyFrame2.getRotation());

				// Y'(t)= Y0 + (X(t) - X0)
				blendTKF->setTranslation(glm::vec3(y0 + (xt - x0)));
				// Yr'(t) = Xr(t) inv(Xr0) Yr0
				blendTKF->setRotation(xrt * inv_xr0 * yr0);
			}
			break;

			case MAP_ADDITIVE:
			{
				baseTrack->getInterpolatedKeyFrame(time, &baseKeyFrame);
				layerTrack->getInterpolatedKeyFrame(time, &layerKeyFrame1);
				layerTrack->getInterpolatedKeyFrame(time - playbackDelta, &layerKeyFrame2);

				const glm::vec3 yt(baseKeyFrame.getTranslation());
				const glm::vec3 xt(layerKeyFrame1.getTranslation());
				const glm::vec3 xt_dt(layerKeyFrame2.getTranslation());

				// Y'(t) = Y(t) + (X(t) - X(t - dt))
				blendTKF->setTranslation(glm::vec3(yt + (xt - xt_dt)));
				blendTKF->setRotation(glm::quat(baseKeyFrame.getRotation()));
			}
			break;

			case MAP_TRAJECTORY_RELATIVE:
			{
				baseTrack->getInterpolatedKeyFrame(0.f, &baseKeyFrame);
				layerTrack->getInterpolatedKeyFrame(time, &layerKeyFrame1);
				layerTrack->getInterpolatedKeyFrame(0.f, &layerKeyFrame2);

				const glm::vec3 boneOffsetPos = layerKeyFrame1.getTranslation() - layerRootPos;

				const glm::vec3 y0(baseKeyFrame.getTranslation());
				const glm::vec3 xt(baseRootPos + boneOffsetPos);
				const glm::vec3 x0(layerKeyFrame2.getTranslation());

				// Y'(t) = Y(t) + K(t)*(X(t) - X0)
				blendTKF->setTranslation(glm::vec3(y0 + (xt - x0)));
				blendTKF->setRotation(glm::quat(baseKeyFrame.getRotation()));
			}
			break;
		}
	}
}

void Recording::updateRecording( float delta )
{
	if (!recording) return;

	// Update animation timer for this new keyframe
	recordingTime += recordingDelta;
	saveKeyFrame(recordingTime);

	// Update gui label text
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage(
		"Mem usage: " + std::to_string(animation->_calcMemoryUsage()) + " bytes\n" ));
}

void Recording::updatePlayback( float delta )
{
	if (!playback) return;

	playbackTime += playbackDelta;

	const float len = animation->getLength();
	if (playbackTime > len) {
		if (looping) playbackTime = 0.f;
		else         playbackTime = len;
	}
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

float Recording::getAnimationLength() const {
	if (nullptr != animation) return animation->getLength();
	else                      return 0.f;
}
