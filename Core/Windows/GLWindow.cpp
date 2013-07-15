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
	, currentAnimation(nullptr)
	, animLayer()
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

	animLayer["base"] = std::unique_ptr<Animation>(new Animation(0, "base"));
	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		animLayer["base"]->createBoneTrack(boneID);
	}
	currentAnimation = animLayer["base"].get();

	animLayer["blend"] = std::unique_ptr<Animation>(new Animation(0, "blend"));
	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		animLayer["blend"]->createBoneTrack(boneID);
	}

	light0.position = glm::vec3(0,1,0);
	light0.intensities = glm::vec3(1,1,1);
	light0.attenuation = 0.01f;
	light0.ambientCoefficient = 0.01f;
}

void GLWindow::update()
{
	handleEvents();

	updateCamera();
	updateTextures();
	updateRecording();
	updatePlayback();

	if (nullptr == currentAnimation) {
		return;
	}

	const float length = currentAnimation->getLength();
	const float progress = (length == 0.f) ? 0.f : playbackTime / length;
	msg::gDispatcher.dispatchMessage(msg::PlaybackSetProgressMessage(progress));

}

float dt = 0.f;
void GLWindow::render()
{
	window.setActive();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0f, 1.0f);

	//glClearColor(0.53f, 0.81f, 0.92f, 1.f); // sky blue
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLUtils::defaultProgram->use();
	GLUtils::defaultProgram->setUniform("camera", camera.matrix());
	GLUtils::defaultProgram->setUniform("tex", 0);
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
	dt += app.getDeltaTime().asSeconds() / 3.f;
	light0.position = glm::vec3(1.f * glm::cos(dt), 0.5f, 2.25f * glm::sin(dt));
	GLUtils::defaultProgram->setUniform("light.position", light0.position);
	GLUtils::defaultProgram->setUniform("light.intensities", light0.intensities);
	GLUtils::defaultProgram->setUniform("light.attenuation", light0.attenuation);
	GLUtils::defaultProgram->setUniform("light.ambientCoefficient", light0.ambientCoefficient);
	glActiveTexture(GL_TEXTURE0);

	// Draw ground plane
	glBindTexture(GL_TEXTURE_2D, gridTexture->object());
	GLUtils::defaultProgram->setUniform("model", glm::translate(glm::mat4(), glm::vec3(0.f, -1.f, 0.f)));
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(20,20));
	Render::plane();

	// Draw live skeleton
	glBindTexture(GL_TEXTURE_2D, colorTexture->object());
	GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
	if (liveSkeletonVisible) {
		app.getKinect().getLiveSkeleton()->render();
	}

	// Draw current animation layer
	if (nullptr != currentAnimation && currentAnimation->getLength() > 0.f) {
		GLUtils::simpleProgram->use();
		GLUtils::simpleProgram->setUniform("camera", camera.matrix());
		GLUtils::simpleProgram->setUniform("color", glm::vec4(0.2f, 1.f, 0.2f, 1.f));

		TransformKeyFrame kf(animTimer.asSeconds(), 0);
		currentAnimation->getBoneTrack(HAND_RIGHT)->getInterpolatedKeyFrame(playbackTime, &kf);
		const float s = 0.025f;
		const glm::vec3 scale(s,s,s);
		const glm::vec3 headPos = kf.getTranslation();
		GLUtils::simpleProgram->setUniform("model", glm::scale(glm::translate(glm::mat4(), headPos), scale * 1.5f));
		glCullFace(GL_FRONT);
		Render::sphere();
		glCullFace(GL_BACK);

		GLUtils::defaultProgram->use();
		GLUtils::defaultProgram->setUniform("camera", camera.matrix());
		GLUtils::defaultProgram->setUniform("light.position", light0.position);
		GLUtils::defaultProgram->setUniform("light.intensities", light0.intensities);
		GLUtils::defaultProgram->setUniform("light.attenuation", light0.attenuation);
		GLUtils::defaultProgram->setUniform("light.ambientCoefficient", light0.ambientCoefficient);
		GLUtils::defaultProgram->setUniform("texscale", glm::vec2(1,1));
		GLUtils::defaultProgram->setUniform("tex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, redTileTexture->object());
		skeleton->render();

		glBindTexture(GL_TEXTURE_2D, 0);
		if (bonePathsVisible) {
			// Get a bone track, just right hand for now
			auto boneTrack = currentAnimation->getBoneTrack(EBoneID::HAND_RIGHT);
			auto numKeyFrames = boneTrack->getNumKeyFrames();
			auto keyFrames = boneTrack->getKeyFrames();

			// Get all translation vectors for this track
			static std::vector<glm::vec3> vertices;
			vertices.resize(numKeyFrames);

			unsigned int i = 0;
			std::for_each(begin(keyFrames), end(keyFrames), [&](KeyFrame *keyframe) {
				if (keyframe->getTime() < playbackTime) {
					vertices[i++] = static_cast<TransformKeyFrame*>(keyframe)->getTranslation();
				}
			});

			// Draw the path
			GLUtils::simpleProgram->use();
			GLUtils::simpleProgram->setUniform("camera", camera.matrix());
			GLUtils::simpleProgram->setUniform("color", glm::vec4(1,0,0,1));
			GLUtils::simpleProgram->setUniform("model", glm::mat4());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			const GLuint vertexAttribLoc = GLUtils::simpleProgram->attrib("vertex");
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableVertexAttribArray(vertexAttribLoc);
			glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(vertices[0]));

			glDrawArrays(GL_LINE_STRIP, 0, i);

			glDisableVertexAttribArray(vertexAttribLoc);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}

	// Draw light
	GLUtils::simpleProgram->use();
	GLUtils::simpleProgram->setUniform("camera", camera.matrix());
	GLUtils::simpleProgram->setUniform("color", glm::vec4(1,0.85f,0,1));
	glBindTexture(GL_TEXTURE_2D, redTileTexture->object());
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
	// Select animation to save keyframe to
	Animation *animation = nullptr;
	     if (recording) animation = animLayer["base"].get();
	else if (layering)  animation = currentAnimation;

	// Can't save if there's nothing to save to
	if (nullptr == animation) {
		return;
	}

	// Update animation timer for this new keyframe
	animTimer += app.getDeltaTime();
	const float now = animTimer.asSeconds();

	size_t numKeyFrames = saveKeyFrame(now, currentAnimation);

	if (layering) {
		if (now < animLayer["base"]->getLength()) {
			saveBlendKeyFrame(now, animLayer["blend"].get());
		} else {
			msg::StopRecordingMessage msg;
			process(&msg);
			MessageBoxA(NULL, "Done recording layer", "Done", MB_OK);
		}
	}

	// Update gui label text
	const std::string text = "Saved " + std::to_string(numKeyFrames) + " key frames\n"
		+ "Mem usage: " + std::to_string(animation->_calcMemoryUsage()) + " bytes\n";
	msg::gDispatcher.dispatchMessage(msg::SetRecordingLabelMessage(text));
}

void GLWindow::updatePlayback()
{
	if (!playbackRunning || nullptr == currentAnimation) {
		return;
	}

	const float anim_length = currentAnimation->getLength();
	playbackTime += playbackDelta;
	if (playbackTime > anim_length) {
		playbackTime = 0.f;
	}

	if (layering) {
		animLayer["blend"]->apply(skeleton.get(), playbackTime);
	} else if (anim_length > 0.f) {
		currentAnimation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::recordLayer()
{
	// Ignore if currently layering or recording, or no animation is selected
	if (layering || recording) return;

	// Need to have a base animation loaded to layer over
	Animation *base = animLayer["base"].get();
	if (nullptr == base || base->getLength() == 0.f) {
		 return;
	}

	layering = true;
	animTimer = sf::Time::Zero;
	std::cout << "now layering...\n";
	// TODO : start countdown timer

	// Create a new animation layer
	const std::string layerName = "layer " + std::to_string(++layerID);

	animLayer[layerName] = std::unique_ptr<Animation>(new Animation(layerID, layerName));
	Animation *newLayer = animLayer[layerName].get();
	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		newLayer->createBoneTrack(boneID);
	}
	currentAnimation = newLayer;

	// Update ui layer combo box
	msg::gDispatcher.dispatchMessage(msg::AddLayerItemMessage(layerName));
}

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
		keyFrame->setTranslation(glm::vec3(pos.x, pos.y, pos.z));
		keyFrame->setRotation(glm::quat(orient.w, orient.x, orient.y, orient.z));
		keyFrame->setScale(glm::vec3(1,1,1));

		numKeyFrames += track->getNumKeyFrames();
	}

	return numKeyFrames;
}

void GLWindow::saveBlendKeyFrame( float now, Animation *animation )
{
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
}

void GLWindow::process( const msg::StartRecordingMessage *message )
{
	recording = true;
}

void GLWindow::process( const msg::StopRecordingMessage *message )
{
	recording = false;
	layering = false;
}

void GLWindow::process( const msg::ClearRecordingMessage *message )
{
	// Handle a clear frames request
	msg::gDispatcher.dispatchMessage(msg::StopRecordingMessage());

	if (nullptr == currentAnimation) {
		return;
	}

	// Clear and recreate all bone tracks
	currentAnimation->deleteAllBoneTrack();
	for (auto boneID = 0; boneID < EBoneID::COUNT; ++boneID) {
		currentAnimation->createBoneTrack(boneID);
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
	if (nullptr != currentAnimation && currentAnimation->getLength() > 0.f) {
		currentAnimation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::PlaybackLastFrameMessage *message )
{
	playbackTime = currentAnimation->getLength();
	if (nullptr != currentAnimation && playbackTime > 0.f) {
		currentAnimation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::PlaybackPrevFrameMessage *message )
{
	playbackTime -= playbackDelta;
	if (playbackTime < 0.f) {
		playbackTime = 0.f;
	}

	if (nullptr != currentAnimation && currentAnimation->getLength() > 0.f) {
		currentAnimation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::PlaybackNextFrameMessage *message )
{
	if (nullptr == currentAnimation) {
		return;
	}
	const float length = currentAnimation->getLength();

	playbackTime += playbackDelta;
	if (playbackTime > length) {
		playbackTime = length;
	}

	if (length > 0.f) {
		currentAnimation->apply(skeleton.get(), playbackTime);
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

void GLWindow::process( const msg::StartLayeringMessage *message )
{
	recordLayer();
}

void GLWindow::process( const msg::LayerSelectMessage *message )
{
	// Lookup selected layer by name; if it exists, set it as current
	auto it = animLayer.find(message->layerName);
	currentAnimation = (end(animLayer) != it) ? it->second.get() : nullptr;

	// Restart playback timer and apply animation to skeleton viz
	animTimer = sf::Time::Zero;
	if (nullptr != currentAnimation && currentAnimation->getLength() > 0.f) {
		playbackTime = 0.f;
		playbackRunning = false;
		currentAnimation->apply(skeleton.get(), playbackTime);
	}
}

void GLWindow::process( const msg::ShowBonePathMessage *message )
{
	bonePathsVisible = true;
}

void GLWindow::process( const msg::HideBonePathMessage *message )
{
	bonePathsVisible = false;
}
