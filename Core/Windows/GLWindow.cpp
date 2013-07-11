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


GLWindow::GLWindow(const std::string& title, App& app)
	: Window(title, app)
	, liveSkeletonVisible(true)
	, playbackRunning(false)
	, playbackTime(0)
	, playbackDelta(1.f / 60.f)
	, animTimer(sf::Time::Zero)
	, camera()
	, colorTexture(nullptr)
	, depthTexture(nullptr)
	, gridTexture(nullptr)
	, capsuleTexture(nullptr)
	, animation(nullptr)
	, skeleton(nullptr)
{
	const sf::Uint32 style = sf::Style::Default;
	const sf::ContextSettings contextSettings(depth_bits, stencil_bits, antialias_level, gl_major_version, gl_minor_version);
	videoMode = sf::VideoMode(sf::VideoMode::getDesktopMode().width  - 300
	                        , sf::VideoMode::getDesktopMode().height - 100, color_bits);
	window.create(videoMode, title, style, contextSettings);
	window.setFramerateLimit(framerate_limit);
	window.setPosition(sf::Vector2i(initial_pos_x, initial_pos_y));

	resetCamera();

	msg::gDispatcher.registerHandler(msg::CLEAR_SKELETON_RECORDING, this);
	msg::gDispatcher.registerHandler(msg::SHOW_LIVE_SKELETON,       this);
	msg::gDispatcher.registerHandler(msg::HIDE_LIVE_SKELETON,       this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_FIRST_FRAME,     this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_LAST_FRAME,      this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_PREV_FRAME,      this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_NEXT_FRAME,      this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_START,           this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_STOP,            this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_SET_DELTA,       this);
}

GLWindow::~GLWindow()
{
	// Nothing to do, yay shared pointers
}

void GLWindow::init()
{
	SetForegroundWindow(window.getSystemHandle());

	loadTextures();

	animation = std::shared_ptr<Animation>(new Animation(0, "test_anim"));
	for (unsigned short boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		animation->createBoneTrack(boneID);
	}

	skeleton = std::shared_ptr<Skeleton>(new Skeleton());
}

void GLWindow::update()
{
	handleEvents();

	updateCamera();
	updateTextures();
	updateRecording();

	const float length = animation->getLength();
	const float progress = (length == 0.f) ? 0.f : playbackTime / length;
	msg::gDispatcher.dispatchMessage(msg::PlaybackSetProgressMessage(progress));

	bool space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
	glPolygonMode(GL_FRONT_AND_BACK, (space ? GL_LINE : GL_FILL));
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

	glClearColor(0.53f, 0.81f, 0.92f, 1.f); // sky blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLUtils::defaultProgram->use();
	GLUtils::defaultProgram->setUniform("camera", camera.matrix());
	GLUtils::defaultProgram->setUniform("tex", 0);
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
	glActiveTexture(GL_TEXTURE0);

	// Draw ground plane
	glBindTexture(GL_TEXTURE_2D, gridTexture->object());
	GLUtils::defaultProgram->setUniform("model", glm::translate(glm::mat4(), glm::vec3(0.f, -1.f, 0.f)));
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(10,10));
	Render::plane();

	// Draw live skeleton
	glBindTexture(GL_TEXTURE_2D, colorTexture->object());
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
	if (liveSkeletonVisible) {
		app.getKinect().getLiveSkeleton()->render();
	}

	// Draw recorded animation
	glBindTexture(GL_TEXTURE_2D, gridTexture->object());
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(2,2));
	if (animation->getLength() > 0.f) {
		skeleton->render();
	}

	glBindTexture(GL_TEXTURE_2D, capsuleTexture->object());
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
	GLUtils::defaultProgram->setUniform("model", glm::translate(glm::mat4(), glm::vec3(0.f, 2.f, 0.f)));
	Render::capsule();

	GLUtils::defaultProgram->stopUsing();

	window.display();
}

void GLWindow::resetCamera()
{
	camera.setViewportAspectRatio((float) videoMode.width / (float) videoMode.height);
	camera.setFieldOfView(66.f);
	camera.setNearAndFarPlanes(0.1f, 100.f);
	camera.setPosition(glm::vec3(0, 2, 10));
	camera.offsetOrientation(-camera.verticalAngle(), -camera.horizontalAngle());
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
				case sf::Keyboard::Return: {
					// Update rendered skeleton data
					// TODO : setup bone mask for all Kinect joints
					const float len = animation->getLength();
					if (len != 0.f) {
						animation->apply(skeleton.get(), playbackTime);
						if ((playbackTime += playbackDelta) > len) playbackTime = 0.f;
					}
				}
				break;
			}
		}
	}
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
	if (playbackRunning) {
		const float anim_length = animation->getLength();

		playbackTime += playbackDelta;
		if (playbackTime > anim_length) {
			playbackTime = 0.f;
		}

		if (anim_length > 0.f) {
			animation->apply(skeleton.get(), playbackTime);
		}
	}

	if (!app.isRecording()) {
		return;
	}

	// Get the Kinect skeleton data if there is any
	const KinectDevice& kinect = app.getKinect();
	const NUI_SKELETON_FRAME& skeletonFrame = kinect.getSkeletonFrame();
	const NUI_SKELETON_DATA *skeletonData = kinect.getFirstTrackedSkeletonData(skeletonFrame);
	const NUI_SKELETON_BONE_ORIENTATION *boneOrientations = kinect.getOrientations();
	if (nullptr == skeletonData) return;

	// Update animation timer for this set of keyframes
	animTimer += app.getDeltaTime();
	const float now = animTimer.asSeconds();

	// Update all bone tracks
	int numKeyFrames = 0;
	for (unsigned short boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		BoneAnimationTrack *track = animation->getBoneTrack(boneID);
		if (nullptr == track) continue;

		TransformKeyFrame *keyFrame = dynamic_cast<TransformKeyFrame*>(track->createKeyFrame(now));
		if (nullptr == keyFrame) continue;

		const Vector4& pos = skeletonData->SkeletonPositions[boneID];
		const Vector4& orient = boneOrientations[boneID].absoluteRotation.rotationQuaternion;
		keyFrame->setTranslation(glm::vec3(pos.x, pos.y, pos.z));
		keyFrame->setRotation(glm::quat(orient.w, orient.x, orient.y, orient.z));
		keyFrame->setScale(glm::vec3(1,1,1));

		numKeyFrames += track->getNumKeyFrames();
	}

	// Update gui label text
	std::stringstream ss;
	ss << "Saved " << numKeyFrames << " key frames\n"
	   << "Mem usage: " << animation->_calcMemoryUsage() << " bytes\n";
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage(ss.str()));
}

void GLWindow::loadTextures()
{
	colorTexture = std::shared_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::BGRA
		                 , KinectDevice::image_stream_width, KinectDevice::image_stream_height
		                 , (unsigned char *) app.getKinect().getColorData()));

	depthTexture = std::shared_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::BGRA
		                 , KinectDevice::image_stream_width, KinectDevice::image_stream_height
		                 , (unsigned char *) app.getKinect().getColorData()));

	sf::Image gridImage(GetImage("grid.png"));
	gridTexture = std::shared_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::RGBA
		                 , gridImage.getSize().x, gridImage.getSize().y
		                 , (unsigned char *) gridImage.getPixelsPtr()
		                 , GL_NEAREST, GL_REPEAT));

	sf::Image capsuleImage(GetImage("capsule.png"));
	capsuleTexture = std::shared_ptr<tdogl::Texture>(
		new tdogl::Texture(tdogl::Texture::Format::RGBA
		                 , capsuleImage.getSize().x, capsuleImage.getSize().y
		                 , (unsigned char *) capsuleImage.getPixelsPtr()
						 , GL_LINEAR, GL_CLAMP_TO_EDGE));
}

void GLWindow::process( const msg::ClearRecordingMessage *message )
{
	// Handle a clear frames request
	msg::gDispatcher.dispatchMessage(msg::StopRecordingMessage());

	// Clear and recreate all bone tracks
	animation->deleteAllBoneTrack();
	for (unsigned short boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		animation->createBoneTrack(boneID);
	}

	// Update gui label
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage("Skeleton Recording:"));
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
	playbackTime = 0.f;
	if (animation->getLength() > 0.f) {
		animation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::PlaybackLastFrameMessage *message )
{
	playbackTime = animation->getLength();
	if (animation->getLength() > 0.f) {
		animation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::PlaybackPrevFrameMessage *message )
{
	playbackTime -= playbackDelta;
	if (playbackTime < 0.f) {
		playbackTime = 0.f;
	}
	if (animation->getLength() > 0.f) {
		animation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::PlaybackNextFrameMessage *message )
{
	playbackTime += playbackDelta;
	if (playbackTime > animation->getLength()) {
		playbackTime = animation->getLength();
	}
	if (animation->getLength() > 0.f) {
		animation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::PlaybackStartMessage *message )
{
	playbackRunning = true;
}

void GLWindow::process( const msg::PlaybackStopMessage *message )
{
	playbackRunning = false;
}

void GLWindow::process( const msg::PlaybackSetDeltaMessage *message )
{
	playbackDelta = message->delta;
}
