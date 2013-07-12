#pragma once
#include "Window.h"
#include "Scene/Camera.h"
#include "Core/Messages/Messages.h"

#include <SFML/System/Time.hpp>

#include <string>
#include <memory>
#include <map>

namespace tdogl { class Texture; }

class Animation;
class Skeleton;


class GLWindow : public Window, msg::Handler
{
public:
	GLWindow(const std::string& title, App& app);
	virtual ~GLWindow();

	void init();
	void update();
	void render();

private:
	void resetCamera();
	void handleEvents();
	void updateCamera();
	void updateRecording();
	void updatePlayback();
	void updateTextures();
	void recordLayer();
	void loadTextures();
	size_t saveKeyFrame(float now, Animation *animation);

private:
	bool liveSkeletonVisible;
	bool playbackRunning;
	bool recording;
	bool layering;

	float playbackTime;
	float playbackDelta;

	unsigned char layerID;

	sf::Time animTimer;

	tdogl::Camera camera;

	std::unique_ptr<tdogl::Texture> colorTexture;
	std::unique_ptr<tdogl::Texture> depthTexture;
	std::unique_ptr<tdogl::Texture> gridTexture;
	std::unique_ptr<tdogl::Texture> redTileTexture;

	std::unique_ptr<Skeleton>  skeleton;

	Animation *currentAnimation;
	std::map< std::string, std::unique_ptr<Animation> > animLayer;

	// Message processing methods ----------------------------
	void registerMessageHandlers();

public:
	void process(const msg::StartRecordingMessage     *message);
	void process(const msg::StopRecordingMessage      *message);
	void process(const msg::ClearRecordingMessage     *message);
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

};
