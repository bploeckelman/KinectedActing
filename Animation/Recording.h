#pragma once

#include "Animation.h"

#include <memory>
#include <string>

class KinectDevice;

class Recording
{
public:
	Recording(const std::string& name, const KinectDevice& kinect);
	~Recording();

	void update(float delta);
	void apply(Skeleton *skeleton, float time, const std::set<EBoneID>& boneMask=default_bone_mask);

	void showBonePaths();
	void hideBonePaths();

	void startPlayback();
	void stopPlayback();

	void startRecording();
	void stopRecording();

	Animation *getAnimation();

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

inline Animation *Recording::getAnimation() { return animation.get(); }
