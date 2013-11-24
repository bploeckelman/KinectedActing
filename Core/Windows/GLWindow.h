#pragma once
#include "Window.h"
#include "Scene/Camera.h"
#include "Core/Messages/Messages.h"
#include "Animation/AnimationTypes.h"

#include <SFML/System/Time.hpp>

#include <string>
#include <memory>
#include <list>
#include <map>

namespace tdogl { class Texture; }

class Animation;
class Skeleton;
class Recording;


class GLWindow : public Window, msg::Handler
{
public:
	GLWindow(const std::string& title, App& app);
	virtual ~GLWindow();

	void init();
	void update();
	void render();

private:
	// Update helpers 
	void handleEvents();
	void updateCamera();
	void updateRecording();
	void updatePlayback();
	void updateTextures();

	// Render helpers
	void renderSetup()        const;
	void renderGroundPlane()  const;
	void renderBasisAxes()    const;
	void renderLiveSkeleton() const;
	void renderCurrentLayer() const;
	void renderBlendLayer()   const;
	void renderLights()       const;

	// Misc helpers
	void resetCamera();
	void recordLayer();
	void loadTextures();

private:
	bool liveSkeletonVisible;
	bool bonePathsVisible;
	bool playbackRunning;
	bool recording;
	bool layering;
	bool baseSaved;

	float playbackTime;
	float playbackDelta;

	unsigned char layerID;

	tdogl::Camera camera;

	std::unique_ptr<tdogl::Texture> colorTexture;
	std::unique_ptr<tdogl::Texture> depthTexture;
	std::unique_ptr<tdogl::Texture> gridTexture;
	std::unique_ptr<tdogl::Texture> redTileTexture;

	std::unique_ptr<Skeleton> selectedSkeleton;
	std::unique_ptr<Skeleton> blendSkeleton;

	BoneMask boneMask;
	ELayerMappingMode mappingMode;

	Recording *currentRecording;
	std::map< std::string, std::unique_ptr<Recording> > recordings;

	// Message processing methods ----------------------------
	void registerMessageHandlers();

public:
	void process(const msg::StartRecordingMessage     *message);
	void process(const msg::StopRecordingMessage      *message);
	void process(const msg::ClearRecordingMessage     *message);
	void process(const msg::ExportSkeletonBVHMessage  *message);
	void process(const msg::ShowLiveSkeletonMessage   *message);
	void process(const msg::HideLiveSkeletonMessage   *message);
	void process(const msg::PlaybackFirstFrameMessage *message);
	void process(const msg::PlaybackLastFrameMessage  *message);
	void process(const msg::PlaybackPrevFrameMessage  *message);
	void process(const msg::PlaybackNextFrameMessage  *message);
	void process(const msg::PlaybackStartMessage      *message);
	void process(const msg::PlaybackStopMessage       *message);
	void process(const msg::PlaybackSetDeltaMessage   *message);
	void process(const msg::StartLayeringMessage      *message);
	void process(const msg::LayerSelectMessage        *message);
	void process(const msg::MappingModeSelectMessage  *message);
	void process(const msg::ShowBonePathMessage       *message);
	void process(const msg::HideBonePathMessage       *message);
	void process(const msg::UpdateBoneMaskMessage     *message);

};
