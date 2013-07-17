#pragma once
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

#include <NuiApi.h>

#include <SFML/System/Clock.hpp>

#include <fstream>
#include <string>
#include <vector>

class Skeleton;


class KinectDevice
{
private:
	enum EStreamType { COLOR_STREAM, DEPTH_STREAM };

	static const NUI_IMAGE_RESOLUTION color_resolution = NUI_IMAGE_RESOLUTION_640x480;
	static const NUI_IMAGE_RESOLUTION depth_resolution = NUI_IMAGE_RESOLUTION_640x480;

public:
	static const int image_stream_width  = 640;
	static const int image_stream_height = 480;
	static const int bytes_per_pixel     = 4;

public:
	KinectDevice();
	~KinectDevice();

	void shutdown();
	bool init();
	void update();

	void toggleSeatedMode();
	void setSkeletonSmoothingParms( float smoothing
	                              , float correction
	                              , float prediction
	                              , float jitterRadius
	                              , float maxDeviationRadius );

	const INuiSensor *getSensor() const;
	const std::string& getDeviceId() const;
	const byte *getColorData() const;
	const byte *getDepthData() const;
	const Skeleton *getLiveSkeleton() const;
	const NUI_SKELETON_FRAME& getSkeletonFrame() const;
	const NUI_SKELETON_BONE_ORIENTATION *getOrientations() const;

	bool isInitialized() const;
	bool isSeatedModeEnabled() const;

	static const NUI_SKELETON_DATA *getFirstTrackedSkeletonData(const NUI_SKELETON_FRAME& skeletonFrame);

public: // External interface
	static void initRequest();

private:
	void checkForColorFrame();
	void checkForDepthFrame();
	void checkForSkeletonFrame();

	HRESULT processImageStreamData(const EStreamType& eStreamType);
	HRESULT processSkeletonData();

private:
	INuiSensor *sensor;
	std::string deviceId;

	HANDLE colorStream;
	HANDLE depthStream;

	byte *colorData;
	byte *depthData;

	Skeleton *liveSkeleton;
	NUI_SKELETON_FRAME skeletonFrame;
	NUI_SKELETON_DATA *skeletonData;
	NUI_SKELETON_BONE_ORIENTATION boneOrientations[NUI_SKELETON_POSITION_COUNT];
	NUI_TRANSFORM_SMOOTH_PARAMETERS skeletonSmoothParams;
	DWORD  skeletonTrackingFlags;

	HANDLE nextColorFrameEvent;
	HANDLE nextDepthFrameEvent;
	HANDLE nextSkeletonFrameEvent;

};

inline void KinectDevice::setSkeletonSmoothingParms( float smoothing
                                                   , float correction
                                                   , float prediction
                                                   , float jitterRadius
                                                   , float maxDeviationRadius )
{
	NUI_TRANSFORM_SMOOTH_PARAMETERS params = { smoothing, correction, prediction, jitterRadius, maxDeviationRadius };
	skeletonSmoothParams = params;
}

inline const INuiSensor *KinectDevice::getSensor() const { return sensor; }
inline const std::string& KinectDevice::getDeviceId() const { return deviceId; }

inline const byte *KinectDevice::getColorData() const { return colorData; }
inline const byte *KinectDevice::getDepthData() const { return depthData; }
inline const Skeleton *KinectDevice::getLiveSkeleton() const { return liveSkeleton; }
inline const NUI_SKELETON_FRAME& KinectDevice::getSkeletonFrame() const { return skeletonFrame; }
inline const NUI_SKELETON_BONE_ORIENTATION *KinectDevice::getOrientations() const { return boneOrientations; }

inline bool KinectDevice::isInitialized()       const { return (nullptr != sensor); }
inline bool KinectDevice::isSeatedModeEnabled() const { return (0 != (skeletonTrackingFlags & NUI_SKELETON_FRAME_FLAG_SEATED_SUPPORT_ENABLED)); }
