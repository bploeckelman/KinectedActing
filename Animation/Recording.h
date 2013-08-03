#pragma once

#include "AnimationTypes.h"

#include <memory>
#include <string>

class KinectDevice;
class Animation;
class Skeleton;

class Recording
{
public:
	Recording(const std::string& name, const KinectDevice& kinect);
	~Recording();

	void update(float delta);
	void apply(Skeleton *skeleton, float time, const BoneMask& boneMask=default_bone_mask);
	void apply(Skeleton *skeleton, const BoneMask& boneMask=default_bone_mask);

	void saveBlendFrame( float time
	                   , const Recording& base
	                   , const Recording& layer
	                   , const BoneMask& boneMask=default_bone_mask
	                   , const ELayerMappingMode& mappingMode=ELayerMappingMode::MAP_ADDITIVE );

	void showBonePaths();
	void hideBonePaths();

	void startPlayback();
	void stopPlayback();

	void resetPlaybackTime();
	void setPlaybackTime(float t);
	void setPlaybackDelta(float dt);
	float getPlaybackTime() const;

	void playbackNextFrame();
	void playbackPreviousFrame();

	void startRecording();
	void stopRecording();
	void clearRecording();

	Animation *getAnimation();
	const Animation *getAnimation() const;
	float getAnimationLength() const;

private:
	void updateRecording(float delta);
	void updatePlayback(float delta);
	size_t saveKeyFrame(float time);

	static unsigned int nextAnimationID;

	const KinectDevice& kinect;

	std::unique_ptr<Animation> animation;

	bool bonepaths;

	bool  playback;
	float playbackTime;
	float playbackDelta;

	bool  recording;
	float recordingTime;
	float recordingDelta;

};

inline void Recording::showBonePaths()  { bonepaths = true;  }
inline void Recording::hideBonePaths()  { bonepaths = false; }
inline void Recording::startPlayback()  { playback  = true;  }
inline void Recording::stopPlayback()   { playback  = false; }
inline void Recording::startRecording() { recording = true;  }
inline void Recording::stopRecording()  { recording = false; }

inline void Recording::resetPlaybackTime() { playbackTime = 0.f; }
inline void Recording::setPlaybackDelta(float dt) { playbackDelta = dt; }
inline float Recording::getPlaybackTime() const { return playbackTime; }

inline Animation *Recording::getAnimation() { return animation.get(); }
inline const Animation *Recording::getAnimation() const { return animation.get(); }
