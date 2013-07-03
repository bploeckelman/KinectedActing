#include "KinectDevice.h"
#include "Animation/Skeleton.h"

#include <NuiApi.h>

#include <SFML/System/Clock.hpp>

#include <glm/glm.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <tchar.h>

using namespace std;

// Type conversion helper functions
const string bstr_to_std_string(const BSTR bstr);
const glm::mat4 matrix4_to_mat4(const Matrix4& m);

const DWORD skeleton_tracking_flags = NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT; 


KinectDevice::KinectDevice()
	: deviceId("[ offline ]")
	, sensor(nullptr)
	, colorStream(INVALID_HANDLE_VALUE)
	, depthStream(INVALID_HANDLE_VALUE)
	, colorData(new byte[image_stream_width * image_stream_height * bytes_per_pixel])
	, depthData(new byte[image_stream_width * image_stream_height * bytes_per_pixel])
	, liveSkeleton(new Skeleton())
	, skeletonFrame()
	, skeletonData(nullptr)
	, skeletonTrackingFlags(skeleton_tracking_flags)
	, nextColorFrameEvent()
	, nextDepthFrameEvent()
	, nextSkeletonFrameEvent()
{}

KinectDevice::~KinectDevice()
{
	delete liveSkeleton;
	delete[] depthData;
	delete[] colorData;
}

bool KinectDevice::init()
{
	nextColorFrameEvent = CreateEventA(NULL, TRUE, FALSE, "Next Color Frame Event");
	nextDepthFrameEvent = CreateEventA(NULL, TRUE, FALSE, "Next Depth Frame Event");
	nextSkeletonFrameEvent = CreateEventA(NULL, TRUE, FALSE, "Next Skeleton Frame Event");

	ZeroMemory(colorData, image_stream_width * image_stream_height * bytes_per_pixel);
	ZeroMemory(depthData, image_stream_width * image_stream_height * bytes_per_pixel);

	sf::Clock timer;

	// Check whether there are any connected sensors
	int numSensors = -1;
	HRESULT hr = NuiGetSensorCount(&numSensors);
	if (FAILED(hr) || numSensors < 1) {
		MessageBoxA(NULL, "Failed to find Kinect sensors.", "Kinect Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Create the main sensor
	// NOTE : this class only supports a single Kinect device
	hr = NuiCreateSensorByIndex(0, &sensor);
	if (FAILED(hr) || nullptr == sensor) {
		MessageBoxA(NULL, "Failed to create Kinect sensor.", "Kinect Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Initialize the main sensor
	hr = sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH
							 | NUI_INITIALIZE_FLAG_USES_COLOR
							 | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if (FAILED(hr)) {
		MessageBoxA(NULL, "Failed to initialize Kinect sensor.", "Kinect Error", MB_OK | MB_ICONERROR);
		return false;
	}
	
	// Open color stream to receive color data
	hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR
	                              , color_resolution
	                              , 0                   // Image stream flags, eg. near mode...
	                              , 2                   // Number of frames to buffer
	                              , nextColorFrameEvent // Event handle
	                              , &colorStream);
	if (FAILED(hr)) {
		MessageBoxA(NULL, "Failed to open color stream for Kinect sensor.", "Kinect Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Open depth stream to receive depth data
	hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH
	                              , depth_resolution
	                              , 0                   // Image stream flags, eg. near mode...
	                              , 2                   // Number of frames to buffer
	                              , nextDepthFrameEvent // Event handle
	                              , &depthStream);
	if (FAILED(hr)) {
		MessageBoxA(NULL, "Failed to open depth stream for Kinect sensor.", "Kinect Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Enable skeleton tracking to receive skeleton data
	hr = sensor->NuiSkeletonTrackingEnable(nextSkeletonFrameEvent, skeletonTrackingFlags);
	if (FAILED(hr)) {
		MessageBoxA(NULL, "Failed to enable skeleton tracking for Kinect sensor.", "Kinect Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Get the device id for this sensor
	deviceId = bstr_to_std_string(sensor->NuiDeviceConnectionId());
	//stringstream ss;
	//ss << "Initialized Kinect in " << timer.getElapsedTime().asSeconds() << " seconds.\n"
	//   << "Connection id: [" << deviceId << "]";
	//MessageBoxA(NULL, ss.str().c_str(), "Kinect Info", MB_OK | MB_ICONINFORMATION);

	return true;
}

void KinectDevice::shutdown()
{
	if (nullptr != sensor) {
		sensor->NuiShutdown();
		sensor = nullptr;
	}

	if (INVALID_HANDLE_VALUE != nextColorFrameEvent) {
		CloseHandle(nextColorFrameEvent);
	}

	if (INVALID_HANDLE_VALUE != nextDepthFrameEvent) {
		CloseHandle(nextDepthFrameEvent);
	}

	if (INVALID_HANDLE_VALUE != nextSkeletonFrameEvent) {
		CloseHandle(nextSkeletonFrameEvent);
	}
}

void KinectDevice::update()
{
	checkForColorFrame();
	checkForDepthFrame();
	checkForSkeletonFrame();
}

void KinectDevice::toggleSeatedMode()
{
	if (isSeatedModeEnabled()) {
		skeletonTrackingFlags |= ~NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
	} else {
		skeletonTrackingFlags |= NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
	}

	HRESULT hr = sensor->NuiSkeletonTrackingEnable(nextSkeletonFrameEvent, skeletonTrackingFlags);
	if (FAILED(hr)) {
		MessageBoxA(NULL, "Kinect failed to toggle skeleton tracking mode.", "Kinect Error", MB_OK | MB_ICONINFORMATION);
	}
}

void KinectDevice::checkForColorFrame()
{
	if (WAIT_OBJECT_0 == WaitForSingleObject(nextColorFrameEvent, 0)) {
		if (FAILED(processImageStreamData(COLOR_STREAM))) {
			MessageBoxA(NULL, "Kinect failed to process color stream data.", "Kinect Error", MB_OK | MB_ICONINFORMATION);
		}
	}
}

void KinectDevice::checkForDepthFrame()
{
	if (WAIT_OBJECT_0 == WaitForSingleObject(nextDepthFrameEvent, 0)) {
		if (FAILED(processImageStreamData(DEPTH_STREAM))) {
			MessageBoxA(NULL, "Kinect failed to process depth stream data.", "Kinect Error", MB_OK | MB_ICONINFORMATION);
		}
	}
}

void KinectDevice::checkForSkeletonFrame()
{
	if (WAIT_OBJECT_0 == WaitForSingleObject(nextSkeletonFrameEvent, 0)) {
		if (FAILED(processSkeletonData())) {
			//cerr << "Kinect failed to process skeleton data.\n";
			//MessageBoxA(NULL, "Kinect failed to process skeleton data.", "Kinect Error", MB_OK | MB_ICONINFORMATION);
		}
	}
}

HRESULT KinectDevice::processImageStreamData( const EStreamType& eStreamType )
{
	if (nullptr == sensor) return E_FAIL;

	// Get the specified stream type handle and data pointer
	HANDLE imageStream;
	byte *imageData;
	switch (eStreamType) {
		case COLOR_STREAM: imageStream = colorStream; imageData = colorData; break;
		case DEPTH_STREAM: imageStream = depthStream; imageData = depthData; break;
	}

	HRESULT hr = S_OK;
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT lockedRect;

	// Get the next image stream frame
	hr = sensor->NuiImageStreamGetNextFrame(imageStream, 0, &imageFrame);
	if (FAILED(hr)) {
		return hr;
	}

	// Lock the frame data, copy the image data if it is valid, then unlock the frame
	INuiFrameTexture *texture = nullptr;
	BOOL nearMode = false;
	if (COLOR_STREAM == eStreamType) {
		texture = imageFrame.pFrameTexture;
	} else if (DEPTH_STREAM == eStreamType) {
		hr = sensor->NuiImageFrameGetDepthImagePixelFrameTexture(depthStream, &imageFrame, &nearMode, &texture);
		if (FAILED(hr)) {
			return hr;
		}
	}

	texture->LockRect(0, &lockedRect, NULL, 0);
	if (lockedRect.Pitch != 0) {
		// Color stream is a straight memcopy
		if (COLOR_STREAM == eStreamType) {
			memcpy(imageData, lockedRect.pBits, lockedRect.size);
		}
		// Depth stream is packed with player index, so depth bits must be unpacked
		else if (DEPTH_STREAM == eStreamType) {
			// Get the min and max reliable depth for the current frame
			int minDepth = (nearMode ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
			int maxDepth = (nearMode ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

			const NUI_DEPTH_IMAGE_PIXEL *pBufferRun = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>(lockedRect.pBits);
			const NUI_DEPTH_IMAGE_PIXEL *pBufferEnd = pBufferRun + (image_stream_width * image_stream_height);

			// Write out depth pixel data to dest buffer
			while (pBufferRun < pBufferEnd) {
				// discard the portion of the depth that contains only the player index
				USHORT depth = pBufferRun->depth;

				// To convert to a byte, we're discarding the most-significant
				// rather than least-significant bits.
				// We're preserving detail, although the intensity will "wrap."
				// Values outside the reliable depth range are mapped to 0 (black).

				// Note: Using conditionals in this loop could degrade performance.
				// Consider using a lookup table instead when writing production code.
				BYTE intensity = static_cast<BYTE>(depth >= minDepth && depth <= maxDepth ? depth % 256 : 0);

				// Write out color bytes...
				*(imageData++) = intensity; // blue
				*(imageData++) = intensity; // green
				*(imageData++) = intensity; // red
				*(imageData++) = 255;       // alpha

				// Increment our index into the Kinect's depth buffer
				++pBufferRun;
			}
		}
	} // end if (lockedRect.pBits != 0)
	texture->UnlockRect(0);

	// Copied image stream data, so release data frame from image stream
	sensor->NuiImageStreamReleaseFrame(imageStream, &imageFrame);

	return hr;
}

HRESULT KinectDevice::processSkeletonData()
{
	if (nullptr == sensor) return E_FAIL;

	// Get the next skeleton frame
	HRESULT hr = sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if (FAILED(hr)) {
		return hr;
	}

	// Get skeleton data for the first tracked skeleton
	for (auto data : skeletonFrame.SkeletonData) {
		if (data.eTrackingState == NUI_SKELETON_TRACKED) {
			skeletonData = &data;
			break;
		}
	}

	// Make sure the data is valid 
	if (nullptr == skeletonData) {
		//MessageBoxA(NULL, "Failed to find tracked skeleton data.", "Kinect Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// Apply skeleton data to live Skeleton object
	for (unsigned short boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		Bone *bone = liveSkeleton->getBone(boneID);
		if (nullptr == bone) continue;

		const Vector4& t = skeletonData->SkeletonPositions[boneID];
		bone->translation = glm::vec3(t.x, t.y, t.z);
		bone->rotation = glm::quat(); // TODO
		bone->scale = glm::vec3(1,1,1);
	}

	return hr;
}

//{
	// TODO : how to timestamp?

	// Get data for the first tracked skeleton
	//const NUI_SKELETON_DATA *skeletonData = nullptr;
	//for (auto i = 0; i < NUI_SKELETON_COUNT; ++i) {
	//	if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
	//		skeletonData = &skeletonFrame.SkeletonData[i];
	//		break;
	//	}
	//}
	//if (skeletonData == nullptr) {
	//	//cerr << "Failed to find tracked skeleton from Kinect sensor.\n";
	//	return;
	//}

	// Set filtering level
	//switch (skeleton.getFilterLevel()) {
	//	case OFF:    break;
	//	case LOW:    NuiTransformSmooth(&skeletonFrame, constants::joint_smooth_params_low);  break;
	//	case MEDIUM: NuiTransformSmooth(&skeletonFrame, constants::joint_smooth_params_med);  break;
	//	case HIGH:   NuiTransformSmooth(&skeletonFrame, constants::joint_smooth_params_high); break;
	//}

	// Get bone orientations for this skeleton's joints
	//NUI_SKELETON_BONE_ORIENTATION boneOrientations[NUI_SKELETON_POSITION_COUNT];
	//HRESULT hr = NuiSkeletonCalculateBoneOrientations(skeletonData, boneOrientations);
	//if (FAILED(hr)) {
	//	cerr << "Kinect failed to calculate bone orientations.\n";
	//}

	// For each joint type...
	//for (auto i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) {
	//	// Get joint data in Kinect API form
	//	const NUI_SKELETON_POSITION_INDEX   positionIndex   = toPositionIndex(i);
	//	const NUI_SKELETON_BONE_ORIENTATION boneOrientation = boneOrientations[positionIndex];
	//	const NUI_SKELETON_POSITION_TRACKING_STATE positionTrackingState = skeletonData->eSkeletonPositionTrackingState[positionIndex];
	//	const Vector4& position = skeletonData->SkeletonPositions[positionIndex];
	//	const Matrix4& matrix4 = boneOrientation.absoluteRotation.rotationMatrix;

	//	// Update the joint frame entry for this joint type
	//	Joint& joint = skeleton.getCurrentJointFrame().joints[toJointType(i)];
	//	joint.position      = glm::vec3(position.x, position.y, position.z);
	//	joint.orientation   = toMat4(matrix4);;
	//	joint.type          = toJointType(i);
	//	joint.trackingState = static_cast<ETrackingState>(positionTrackingState);
	//}
//}


const NUI_SKELETON_DATA *KinectDevice::getFirstTrackedSkeletonData(const NUI_SKELETON_FRAME& skeletonFrame)
{
	for (int i = 0; i < NUI_SKELETON_COUNT; ++i) {
		const NUI_SKELETON_DATA *data = &skeletonFrame.SkeletonData[i];
		if (data->eTrackingState == NUI_SKELETON_TRACKED) {
			return data;
		}
	}

	return nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// TODO : move these type conversion helpers to a utility namespace

const string bstr_to_std_string( const BSTR bstr )
{
	const wstring wstr(bstr, SysStringLen(bstr));
	string str; str.assign(wstr.begin(), wstr.end());
	return str;
}

const glm::mat4 matrix4_to_mat4( const Matrix4& m )
{
	return glm::mat4(
		  m.M11, m.M12, m.M13, m.M14
		, m.M21, m.M22, m.M23, m.M24
		, m.M31, m.M32, m.M33, m.M34
		, m.M41, m.M42, m.M43, m.M44);
}
