#include "GLWindow.h"
#include "Core/App.h"
#include "Core/Resources/Texture.h"
#include "Core/Resources/ImageManager.h"
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
#include <SFML/System/Clock.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

static const int color_bits      = 32;
static const int depth_bits      = 24;
static const int stencil_bits    = 8;
static const int antialias_level = 4;
static const int gl_major_version = 3;
static const int gl_minor_version = 3;
static const int framerate_limit = 60;
static const int initial_pos_x   = 260;
static const int initial_pos_y   = 10;

static glm::vec2 mouse_pos_current;

static sf::Clock timer;


GLWindow::GLWindow(const std::string& title, App& app)
	: Window(title, app)
	, camera()
	, colorTexture(nullptr)
	, depthTexture(nullptr)
	, animation(nullptr)
{
	const sf::Uint32 style = sf::Style::Default;
	const sf::ContextSettings contextSettings(depth_bits, stencil_bits, antialias_level, gl_major_version, gl_minor_version);
	videoMode = sf::VideoMode(sf::VideoMode::getDesktopMode().width  - 300
	                        , sf::VideoMode::getDesktopMode().height - 100, color_bits);
	window.create(videoMode, title, style, contextSettings);
	window.setFramerateLimit(framerate_limit);
	window.setPosition(sf::Vector2i(initial_pos_x, initial_pos_y));

	resetCamera();
}

GLWindow::~GLWindow()
{
	delete animation;
	delete colorTexture;
	delete depthTexture;
}

void GLWindow::init()
{
	SetForegroundWindow(window.getSystemHandle());

	colorTexture = new tdogl::Texture(tdogl::Texture::Format::BGRA
	                                , KinectDevice::image_stream_width, KinectDevice::image_stream_height
	                                , (unsigned char *) app.getKinect().getColorData()); 
	depthTexture = new tdogl::Texture(tdogl::Texture::Format::BGRA
	                                , KinectDevice::image_stream_width, KinectDevice::image_stream_height
	                                , (unsigned char *) app.getKinect().getColorData());

	animation = new Animation(0, "test_anim");
	animation->createBoneTrack(EBoneID::HIP_CENTER);
	timer.restart();
}

void GLWindow::update()
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

	updateCamera();

	KinectDevice& kinect = app.getKinect();
	unsigned char *colorData = (unsigned char*) kinect.getColorData();
	unsigned char *depthData = (unsigned char*) kinect.getDepthData();
	const NUI_SKELETON_FRAME& skeletonFrame = kinect.getSkeletonFrame();

	// Update kinect image stream textures
	colorTexture->subImage2D(colorData, KinectDevice::image_stream_width, KinectDevice::image_stream_height);
	depthTexture->subImage2D(depthData, KinectDevice::image_stream_width, KinectDevice::image_stream_height);

	// Update skeleton data
	//const NUI_SKELETON_DATA *skeletonData = kinect.getFirstTrackedSkeletonData(skeletonFrame);
	//if (nullptr == skeletonData) return;
	//if (nullptr == animation)    return;

	//BoneAnimationTrack *hipTrack = animation->getBoneTrack(EBoneID::HIP_CENTER);
	//KeyFrame           *kf       = hipTrack->createKeyFrame(timer.getElapsedTime().asSeconds());
	//TransformKeyFrame  *keyFrame = dynamic_cast<TransformKeyFrame*>(kf);
	//const Vector4&      pos      = skeletonData->Position;
	//keyFrame->setTranslation(glm::vec3(pos.x, pos.y, pos.z));
	//keyFrame->setRotation(glm::quat());
	//keyFrame->setScale(glm::vec3(1,1,1));
}

float d = 0.f; // temporary, for rotating cube
void GLWindow::render()
{
	window.setActive();

	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLUtils::defaultProgram->use();
	GLUtils::defaultProgram->setUniform("camera", camera.matrix());
	GLUtils::defaultProgram->setUniform("tex", 0);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, colorTexture->object());
	glm::mat4 model_matrix;
	model_matrix = glm::translate(glm::mat4(), glm::vec3(0, -5, 0));
	model_matrix = glm::scale(model_matrix, glm::vec3(5,1,5));
	model_matrix = glm::rotate(model_matrix, -90.f, glm::vec3(1,0,0));
	GLUtils::defaultProgram->setUniform("model", model_matrix);
	Render::quad();

	glBindTexture(GL_TEXTURE_2D, depthTexture->object());
	model_matrix = glm::translate(glm::mat4(), glm::vec3(0, 0, -10));
	model_matrix = glm::rotate(model_matrix, d, glm::vec3(0,1,0));
	model_matrix = glm::rotate(model_matrix, d, glm::vec3(0,0,1));
	GLUtils::defaultProgram->setUniform("model", model_matrix);
	Render::quad();
	d += 2.5f;

	const NUI_SKELETON_FRAME& skeletonFrame = app.getKinect().getSkeletonFrame();
	const NUI_SKELETON_DATA  *skeleton = app.getKinect().getFirstTrackedSkeletonData(skeletonFrame);
	if (nullptr != skeleton) {
		for (auto pos : skeleton->SkeletonPositions) {
			model_matrix = glm::translate(glm::mat4(), glm::vec3(pos.x, pos.y, pos.z));
			model_matrix = glm::scale(model_matrix, glm::vec3(0.1f,0.1f,0.1f));
			GLUtils::defaultProgram->setUniform("model", model_matrix);
			Render::quad();
		}
	}

	GLUtils::defaultProgram->stopUsing();

	window.display();
}

void GLWindow::resetCamera()
{
	camera.setViewportAspectRatio((float) videoMode.width / (float) videoMode.height);
	camera.setFieldOfView(66.f);
	camera.setNearAndFarPlanes(0.1f, 100.f);
	camera.setPosition(glm::vec3(0, 0, 20));
	camera.offsetOrientation(-camera.verticalAngle(), -camera.horizontalAngle());
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
	const float dist = 1.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) camera.offsetPosition(forward        * -dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) camera.offsetPosition(forward        *  dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) camera.offsetPosition(camera.right() * -dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) camera.offsetPosition(camera.right() *  dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) camera.offsetPosition(world_up       * -dist);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) camera.offsetPosition(world_up       *  dist);
}
