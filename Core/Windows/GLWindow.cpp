#include "GLWindow.h"
#include "Core/App.h"
#include "Core/Resources/Texture.h"
#include "Core/Resources/ImageManager.h"
#include "Core/Messages/Messages.h"
#include "Kinect/KinectDevice.h"
#include "Scene/Camera.h"
#include "Shaders/Shader.h"
#include "Shaders/Program.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"
#include "Animation/Animation.h"
#include "Animation/BoneAnimationTrack.h"
#include "Animation/TransformKeyFrame.h"
#include "Animation/Recording.h"
#include "Animation/AnimationUtils.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

static const int color_bits      = 32;
static const int depth_bits      = 24;
static const int stencil_bits    = 8;
static const int antialias_level = 8;
static const int gl_major_version = 3;
static const int gl_minor_version = 3;
static const int framerate_limit = 60;
static const int initial_pos_x   = 260;
static const int initial_pos_y   = 5;

static glm::vec2 mouse_pos_current;


struct Light
{
	glm::vec3 position;
	glm::vec3 intensities;
	float attenuation;
	float ambientCoefficient;
};
static struct Light light0;


GLWindow::GLWindow(const std::string& title, App& app)
	: Window(title, app)
	, liveSkeletonVisible(true)
	, bonePathsVisible(false)
	, playbackRunning(false)
	, recording(false)
	, layering(false)
	, playbackTime(0)
	, playbackDelta(1.f / 60.f)
	, layerID(0)
	, animTimer(sf::Time::Zero)
	, camera()
	, colorTexture(nullptr)
	, depthTexture(nullptr)
	, gridTexture(nullptr)
	, redTileTexture(nullptr)
	, skeleton(nullptr)
	, currentRecording(nullptr)
	, recordings()
	, boneMask(default_bone_mask)
{
	const sf::Uint32 style = sf::Style::Default;
	const sf::ContextSettings contextSettings(depth_bits, stencil_bits, antialias_level, gl_major_version, gl_minor_version);
	videoMode = sf::VideoMode(sf::VideoMode::getDesktopMode().width  - 300
	                        , sf::VideoMode::getDesktopMode().height - 100, color_bits);
	window.create(videoMode, title, style, contextSettings);
	window.setFramerateLimit(framerate_limit);
	window.setPosition(sf::Vector2i(initial_pos_x, initial_pos_y));

	resetCamera();
	registerMessageHandlers();
}

GLWindow::~GLWindow()
{
	// Nothing to do, yay unique pointers
}

void GLWindow::init()
{
	SetForegroundWindow(window.getSystemHandle());

	loadTextures();

	skeleton = std::unique_ptr<Skeleton>(new Skeleton());

	recordings["base"]  = std::unique_ptr<Recording>(new Recording("base",  app.getKinect()));
	recordings["blend"] = std::unique_ptr<Recording>(new Recording("blend", app.getKinect()));

	currentRecording = recordings["base"].get();

	light0.position = glm::vec3(0,1,0);
	light0.intensities = glm::vec3(1,1,1);
	light0.attenuation = 0.01f;
	light0.ambientCoefficient = 0.01f;
}

float dt = 0.f;
void GLWindow::update()
{
	handleEvents();

	updateCamera();
	updateTextures();

	updateRecording();
	updatePlayback();

	dt += app.getDeltaTime().asSeconds() / 3.f;
	light0.position = glm::vec3(1.f * glm::cos(dt), 0.5f, 2.25f * glm::sin(dt));

	// Update current recording
	if (nullptr == currentRecording) return;
	currentRecording->update(app.getDeltaTime().asSeconds());
	currentRecording->apply(skeleton.get());

	// Update gui playback progress bar
	const float totalLength = currentRecording->getAnimation()->getLength();
	const float currentTime = currentRecording->getPlaybackTime();
	const float progress = (totalLength == 0.f) ? 0.f : currentTime / totalLength;
	msg::gDispatcher.dispatchMessage(msg::PlaybackSetProgressMessage(progress));

}

void GLWindow::render()
{
	window.setActive();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0f, 1.0f);

	glLineWidth(5.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearColor(0.53f, 0.81f, 0.92f, 1.f); // sky blue
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set uniforms
	GLUtils::defaultProgram->use();
	GLUtils::defaultProgram->setUniform("camera", camera.matrix());
	GLUtils::defaultProgram->setUniform("light.position",           light0.position);
	GLUtils::defaultProgram->setUniform("light.intensities",        light0.intensities);
	GLUtils::defaultProgram->setUniform("light.attenuation",        light0.attenuation);
	GLUtils::defaultProgram->setUniform("light.ambientCoefficient", light0.ambientCoefficient);
	GLUtils::defaultProgram->setUniform("color", glm::vec4(1));
	GLUtils::defaultProgram->setUniform("tex", 0);
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
	glActiveTexture(GL_TEXTURE0);

	// Draw ground plane -------------------------------------------------------
	glBindTexture(GL_TEXTURE_2D, gridTexture->object());
	GLUtils::defaultProgram->setUniform("model", glm::translate(glm::mat4(), glm::vec3(0.f, -1.2f, 0.f)));
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(20,20));
	GLUtils::defaultProgram->setUniform("useLighting", 1);
	Render::plane();

	// Draw an orientation axis at the origin ----------------------------------
	GLUtils::defaultProgram->setUniform("model", glm::translate(glm::mat4(), glm::vec3(0, -1.2f, 0)));
	GLUtils::defaultProgram->setUniform("useLighting", 0);
	Render::axis();

	// Draw live skeleton ------------------------------------------------------
	glBindTexture(GL_TEXTURE_2D, redTileTexture->object());
	if (liveSkeletonVisible) {
		GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
		GLUtils::defaultProgram->setUniform("useLighting", 1);
		app.getKinect().getLiveSkeleton()->render();
	}

	// Draw current animation layer --------------------------------------------
	if (nullptr != currentRecording && currentRecording->getAnimationLength() > 0.f) {
		if (bonePathsVisible) {
			const Animation *animation = currentRecording->getAnimation();
			const float playback_time  = currentRecording->getPlaybackTime();

			std::vector<glm::vec3> positions;

			// Draw bone paths for joints that are enabled in the bone mask ----
			GLUtils::defaultProgram->setUniform("useLighting", 0);
			GLUtils::defaultProgram->setUniform("color", glm::vec4(0,1,0,0.75f));
			for (const auto& boneID : boneMask) {
				animation->getPositions(boneID, positions, currentRecording->getPlaybackTime());
				Render::pipe(positions);
			}

			// TODO : highlight on live skeleton when layering
			// Highlight joints that are enabled in the bone mask --------------
			GLUtils::simpleProgram->use();
			GLUtils::simpleProgram->setUniform("camera", camera.matrix());
			GLUtils::simpleProgram->setUniform("color", glm::vec4(1.f, 0.843f, 0.f, 1.f));
			for (const auto& boneID : boneMask) {
				TransformKeyFrame kf(playback_time, 0);
				animation->getBoneTrack(boneID)->getInterpolatedKeyFrame(playback_time, &kf);

				const glm::vec3 scale(0.025f);
				const glm::vec3 pos = kf.getTranslation();
				GLUtils::simpleProgram->setUniform("model", glm::scale(glm::translate(glm::mat4(), pos), scale * 1.5f));

				glCullFace(GL_FRONT);
				Render::sphere();
				glCullFace(GL_BACK);
			}
		}

		// Draw the skeleton ---------------------------------------------------
		GLUtils::defaultProgram->use();
		GLUtils::defaultProgram->setUniform("camera", camera.matrix());
		GLUtils::defaultProgram->setUniform("light.position", light0.position);
		GLUtils::defaultProgram->setUniform("light.intensities", light0.intensities);
		GLUtils::defaultProgram->setUniform("light.attenuation", light0.attenuation);
		GLUtils::defaultProgram->setUniform("light.ambientCoefficient", light0.ambientCoefficient);
		GLUtils::defaultProgram->setUniform("color", glm::vec4(1));
		GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
		GLUtils::defaultProgram->setUniform("tex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, redTileTexture->object());
		skeleton->render();
	}

	// Draw light --------------------------------------------------------------
	GLUtils::simpleProgram->use();
	GLUtils::simpleProgram->setUniform("camera", camera.matrix());
	GLUtils::simpleProgram->setUniform("color", glm::vec4(1,0.85f,0,1));

	glm::mat4 model;
	model = glm::translate(glm::mat4(), light0.position);
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	GLUtils::simpleProgram->setUniform("model", model);
	Render::sphere();

	glUseProgram(0);

	window.display();
}

void GLWindow::resetCamera()
{
	camera.setViewportAspectRatio((float) videoMode.width / (float) videoMode.height);
	camera.setFieldOfView(66.f);
	camera.setNearAndFarPlanes(0.1f, 100.f);
	camera.setPosition(glm::vec3(0, 1, 3));
	camera.offsetOrientation(33.f - camera.verticalAngle(), -camera.horizontalAngle());
}

void GLWindow::handleEvents()
{
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
			break;
		}
		if (event.type == sf::Event::MouseWheelMoved) {
			camera.setFieldOfView(camera.fieldOfView() - event.mouseWheel.delta);
		}
		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
				case sf::Keyboard::Escape: window.close(); break;
				case sf::Keyboard::BackSpace: resetCamera(); break;
			}
		}
	}

	bool space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
	glPolygonMode(GL_FRONT_AND_BACK, (space ? GL_LINE : GL_FILL));
}

void GLWindow::updateCamera()
{
	// Update mouse position and apply mouse movement to camera orientation
	const sf::Vector2f mousePos(sf::Mouse::getPosition(window));
	const glm::vec2 window_center(window.getSize().x / 2.f, window.getSize().y / 2.f);
	const glm::vec2 mouse_pos_current(mousePos.x, mousePos.y);
	const glm::vec2 mouse_pos_diff(mouse_pos_current - window_center);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		const float threshold = 300.f; // central dead zone radius
		const float rate = 2.f;
		if (glm::length(mouse_pos_diff) > threshold) {
			camera.offsetOrientation(rate * mouse_pos_diff.y / threshold   // up angle offset
								   , rate * mouse_pos_diff.x / threshold); // right angle offset
		}
	}

	// Apply keyboard movement to camera position
	const glm::vec3 forward(camera.forward().x, 0, camera.forward().z);
	const glm::vec3 world_up(0,1,0);
	const float dist = 0.1f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) camera.offsetPosition(forward        * -dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) camera.offsetPosition(forward        *  dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) camera.offsetPosition(camera.right() * -dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) camera.offsetPosition(camera.right() *  dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) camera.offsetPosition(world_up       * -dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) camera.offsetPosition(world_up       *  dist);
}

void GLWindow::updateTextures()
{
	unsigned char *colorData = (unsigned char*) app.getKinect().getColorData();
	unsigned char *depthData = (unsigned char*) app.getKinect().getDepthData();

	// Update kinect image stream textures
	colorTexture->subImage2D(colorData, KinectDevice::image_stream_width, KinectDevice::image_stream_height);
	depthTexture->subImage2D(depthData, KinectDevice::image_stream_width, KinectDevice::image_stream_height);
}

void GLWindow::updateRecording()
{
	// -------------------------------------------------------------------------
	// -------------------------------------------------------------------------
	// TODO : switch from currentAnimation to currentRecording -----------------
	// -------------------------------------------------------------------------
	// -------------------------------------------------------------------------

	// Select recording to save keyframe to
	Recording *record = nullptr;
	if (recording) {
		record = recordings["base"].get();
	} else if (layering) {
		record = currentRecording;
	}
	if (nullptr == record) return;

	// Update animation timer for this new keyframe
	animTimer += app.getDeltaTime();
	const float now = animTimer.asSeconds();

	record->update(now);
	size_t numKeyFrames = record->getAnimation()->getBoneTrack(HEAD)->getNumKeyFrames();
	//size_t numKeyFrames = saveKeyFrame(now, record->getAnimation());

	if (layering) {
		if (now < recordings["base"]->getAnimationLength()) {
			// TODO : blend 'current' and 'base' recordings into 'blend'
			saveBlendKeyFrame(now, recordings["blend"]->getAnimation());
		} else {
			msg::StopRecordingMessage msg;
			process(&msg);
			MessageBoxA(NULL, "Done recording layer", "Done", MB_OK);
		}
	}

	// Update gui label text
	const std::string text = "Saved " + std::to_string(numKeyFrames) + " key frames\n"
		+ "Mem usage: " + std::to_string(record->getAnimation()->_calcMemoryUsage()) + " bytes\n";
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage(text));
}

void GLWindow::updatePlayback()
{
	if (!playbackRunning || nullptr == currentRecording) return;

	if (layering) {
		recordings["blend"]->apply(skeleton.get());
	} else if (currentRecording->getAnimationLength() > 0.f) {
		currentRecording->apply(skeleton.get());
	}
}

void GLWindow::recordLayer()
{
	// Ignore if currently layering or recording
	if (layering || recording) return;

	// Ignore if there isn't a base animation loaded to layer over
	auto base = recordings.find("base");
	if (base == end(recordings) || base->second->getAnimationLength() == 0.f) {
		return;
	}

	layering = true;
	animTimer = sf::Time::Zero;
	// TODO : start countdown timer
	MessageBoxA(NULL, "Start recording a new layer...", "New Layer", MB_OK);

	// Create a new animation layer
	const std::string layerName = "layer " + std::to_string(++layerID);
	recordings[layerName] = std::unique_ptr<Recording>(new Recording(layerName, app.getKinect()));
	currentRecording = recordings[layerName].get();

	// Update ui layer combo box
	msg::gDispatcher.dispatchMessage(msg::AddLayerItemMessage(layerName));
}

/************************************************************************/
/************************************************************************/
/* TODO : can probably remove, should be handled in Recording::update() */
/************************************************************************/
/************************************************************************/
size_t GLWindow::saveKeyFrame( float now, Animation *animation)
{
	if (nullptr == animation) {
		return 0;
	}

	// Get the Kinect skeleton data if there is any
	const KinectDevice& kinect = app.getKinect();
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

		const Vector4& pos = skeletonData->SkeletonPositions[boneID];
		const Vector4& orient = boneOrientations[boneID].absoluteRotation.rotationQuaternion;
		//const Vector4& orient = boneOrientations[boneID].hierarchicalRotation.rotationQuaternion;
		keyFrame->setTranslation(glm::vec3(pos.x, pos.y, pos.z));
		keyFrame->setRotation(glm::quat(orient.w, orient.x, orient.y, orient.z));
		keyFrame->setScale(glm::vec3(1,1,1));

		numKeyFrames += track->getNumKeyFrames();
	}

	return numKeyFrames;
}

void GLWindow::saveBlendKeyFrame( float now, Animation *animation )
{
	// -------------------------------------------------------------------------
	// -------------------------------------------------------------------------
	// TODO : switch from currentAnimation to currentRecording -----------------
	// -------------------------------------------------------------------------
	// -------------------------------------------------------------------------

	/*
	Animation *base = animLayer["base"].get();
	Animation *blend = animLayer["blend"].get();
	BoneAnimationTrack *baseTrack = nullptr;
	BoneAnimationTrack *animTrack = nullptr;
	BoneAnimationTrack *blendTrack = nullptr;
	TransformKeyFrame baseKeyFrame(now, 0);
	TransformKeyFrame animKeyFrame1(now, 0);
	TransformKeyFrame animKeyFrame2(now - playbackDelta, 0);

	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		baseTrack = base->getBoneTrack(boneID);
		animTrack = currentAnimation->getBoneTrack(boneID);
		blendTrack = blend->getBoneTrack(boneID);
		if (nullptr == baseTrack || nullptr == animTrack || nullptr == blendTrack) continue;

		TransformKeyFrame *blendTKF = static_cast<TransformKeyFrame*>(blendTrack->createKeyFrame(now));
		if (nullptr == blendTKF) continue;

		baseTrack->getInterpolatedKeyFrame(now, &baseKeyFrame);
		animTrack->getInterpolatedKeyFrame(now, &animKeyFrame1);
		animTrack->getInterpolatedKeyFrame(now - playbackDelta, &animKeyFrame2);

		const glm::vec3 basePos(baseKeyFrame.getTranslation());
		const glm::vec3 animPos1(animKeyFrame1.getTranslation());
		const glm::vec3 animPos2(animKeyFrame2.getTranslation());

		blendTKF->setTranslation(glm::vec3(basePos + (animPos1 - animPos2)));
		blendTKF->setRotation(glm::quat(baseKeyFrame.getRotation()));
		blendTKF->setScale(glm::vec3(1,1,1));
	}
	*/
}

void GLWindow::loadTextures()
{
	colorTexture = std::unique_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::BGRA
		                 , KinectDevice::image_stream_width, KinectDevice::image_stream_height
		                 , (unsigned char *) app.getKinect().getColorData()));

	depthTexture = std::unique_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::BGRA
		                 , KinectDevice::image_stream_width, KinectDevice::image_stream_height
		                 , (unsigned char *) app.getKinect().getColorData()));

	sf::Image gridImage(GetImage("grid.png"));
	gridTexture = std::unique_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::RGBA
		                 , gridImage.getSize().x, gridImage.getSize().y
		                 , (unsigned char *) gridImage.getPixelsPtr()
		                 , GL_NEAREST, GL_REPEAT));

	sf::Image redTileImage(GetImage("texture.png"));
	redTileTexture = std::unique_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::RGBA
		                 , redTileImage.getSize().x, redTileImage.getSize().y
		                 , (unsigned char *) redTileImage.getPixelsPtr()
						 , GL_LINEAR, GL_CLAMP_TO_EDGE));
}


// ----------------------------------------------------------------------------
// Message processing methods -------------------------------------------------
// ----------------------------------------------------------------------------
void GLWindow::registerMessageHandlers()
{
	msg::gDispatcher.registerHandler(msg::START_SKELETON_RECORDING, this);
	msg::gDispatcher.registerHandler(msg::STOP_SKELETON_RECORDING,  this);
	msg::gDispatcher.registerHandler(msg::CLEAR_SKELETON_RECORDING, this);
	msg::gDispatcher.registerHandler(msg::EXPORT_SKELETON_BVH,      this);
	msg::gDispatcher.registerHandler(msg::SHOW_LIVE_SKELETON,       this);
	msg::gDispatcher.registerHandler(msg::HIDE_LIVE_SKELETON,       this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_FIRST_FRAME,     this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_LAST_FRAME,      this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_PREV_FRAME,      this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_NEXT_FRAME,      this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_START,           this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_STOP,            this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_SET_DELTA,       this);
	msg::gDispatcher.registerHandler(msg::START_LAYERING,           this);
	msg::gDispatcher.registerHandler(msg::LAYER_SELECT,             this);
	msg::gDispatcher.registerHandler(msg::SHOW_BONE_PATH,           this);
	msg::gDispatcher.registerHandler(msg::HIDE_BONE_PATH,           this);
	msg::gDispatcher.registerHandler(msg::UPDATE_BONE_MASK,         this);
}

void GLWindow::process( const msg::StartRecordingMessage *message )
{
	recording = true;

	if (nullptr != currentRecording) {
		currentRecording->startRecording();
	}
}

void GLWindow::process( const msg::StopRecordingMessage *message )
{
	recording = false;
	layering = false;

	// TODO : how to handle layering flag?
	if (nullptr != currentRecording) {
		currentRecording->stopRecording();
	}
}

void GLWindow::process( const msg::ClearRecordingMessage *message )
{
	// Handle a clear frames request
	msg::gDispatcher.dispatchMessage(msg::StopRecordingMessage());

	// Clear and recreate all bone tracks
	if (nullptr != currentRecording) {
		currentRecording->clearRecording();
	}

	// Update gui label
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage("Skeleton Recording:"));
}

void GLWindow::process( const msg::ExportSkeletonBVHMessage *message )
{
	if (nullptr == currentRecording) return;

	exportAnimationAsBVH(currentRecording->getAnimation());

	const std::string text = "Exported recording as '" + currentRecording->getAnimation()->getName() + ".bvh'";
	MessageBoxA(NULL, text.c_str(), "BVH Export", MB_OK);
}

void GLWindow::process( const msg::ShowLiveSkeletonMessage *message )
{
	liveSkeletonVisible = true;
}

void GLWindow::process( const msg::HideLiveSkeletonMessage *message )
{
	liveSkeletonVisible = false;
}

void GLWindow::process( const msg::PlaybackFirstFrameMessage *message )
{
	if (nullptr != currentRecording) {
		currentRecording->resetPlaybackTime();
		currentRecording->apply(skeleton.get());
	}
}

void GLWindow::process( const msg::PlaybackLastFrameMessage *message )
{
	if (nullptr != currentRecording) {
		currentRecording->setPlaybackTime(currentRecording->getAnimationLength());
		currentRecording->apply(skeleton.get());
	}
}

void GLWindow::process( const msg::PlaybackPrevFrameMessage *message )
{
	if (nullptr != currentRecording) {
		currentRecording->playbackPreviousFrame();
		currentRecording->apply(skeleton.get());
	}
}

void GLWindow::process( const msg::PlaybackNextFrameMessage *message )
{
	if (nullptr != currentRecording) {
		currentRecording->playbackNextFrame();
		currentRecording->apply(skeleton.get());
	}
}

void GLWindow::process( const msg::PlaybackStartMessage *message )
{
	playbackRunning = true;

	if (nullptr != currentRecording) {
		currentRecording->startPlayback();
		currentRecording->apply(skeleton.get());
	}
}

void GLWindow::process( const msg::PlaybackStopMessage *message )
{
	playbackRunning = false;

	if (nullptr != currentRecording) {
		currentRecording->stopPlayback();
		currentRecording->apply(skeleton.get());
	}
}

void GLWindow::process( const msg::PlaybackSetDeltaMessage *message )
{
	playbackDelta = message->delta;

	if (nullptr != currentRecording) {
		currentRecording->setPlaybackDelta(message->delta);
	}
}

void GLWindow::process( const msg::StartLayeringMessage *message )
{
	recordLayer();
}

void GLWindow::process( const msg::LayerSelectMessage *message )
{
	// Lookup selected recording by name; if it exists, set it as current
	auto it = recordings.find(message->layerName);
	currentRecording = (end(recordings) != it) ? it->second.get() : nullptr;

	// Restart playback timer and apply animation to skeleton viz
	animTimer = sf::Time::Zero;
	if (nullptr != currentRecording) {
		currentRecording->resetPlaybackTime();
		currentRecording->apply(skeleton.get());
		playbackRunning = false;
	}
}

void GLWindow::process( const msg::ShowBonePathMessage *message )
{
	bonePathsVisible = true;

	if (nullptr != currentRecording) {
		currentRecording->showBonePaths();
	}
}

void GLWindow::process( const msg::HideBonePathMessage *message )
{
	bonePathsVisible = false;

	if (nullptr != currentRecording) {
		currentRecording->hideBonePaths();
	}
}

void GLWindow::process( const msg::UpdateBoneMaskMessage *message )
{
	boneMask = message->boneMask;
}
