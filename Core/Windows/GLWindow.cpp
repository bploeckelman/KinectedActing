#include "GLWindow.h"
#include "Core/App.h"
#include "Kinect/KinectDevice.h"
#include "Scene/Camera.h"
#include "Shaders/Shader.h"
#include "Shaders/Program.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

static const int color_bits      = 32;
static const int depth_bits      = 24;
static const int stencil_bits    = 8;
static const int antialias_level = 4;
static const int framerate_limit = 60;
static const int initial_pos_x   = 260;
static const int initial_pos_y   = 10;

static glm::vec2 mouse_pos_current;


GLWindow::GLWindow(const std::string& title, App& app)
	: Window(title, app)
	, camera()
{
	const sf::Uint32 style = sf::Style::Default;
	const sf::ContextSettings contextSettings(depth_bits, stencil_bits, antialias_level);
	videoMode = sf::VideoMode(sf::VideoMode::getDesktopMode().width  - 300
	                        , sf::VideoMode::getDesktopMode().height - 100, color_bits);
	window.create(videoMode, title, style, contextSettings);
	window.setFramerateLimit(framerate_limit);
	window.setPosition(sf::Vector2i(initial_pos_x, initial_pos_y));

	resetCamera();
}

GLWindow::~GLWindow()
{}

void GLWindow::init()
{
	SetForegroundWindow(window.getSystemHandle());
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
}

float d = 0.f; // temporary, for rotating cube
void GLWindow::render()
{
	window.setActive();
	window.pushGLStates();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // because swank

	GLUtils::defaultProgram->use();
	GLUtils::defaultProgram->setUniform("camera", camera.matrix());

	// Draw a representation of the kinect
	const NUI_SKELETON_FRAME& skeletonFrame = app.getKinect().getSkeletonFrame();
	glm::mat4 kinect_model_matrix;
	kinect_model_matrix = glm::translate(glm::mat4(), glm::vec3(0, skeletonFrame.vFloorClipPlane.w, 0));
	kinect_model_matrix = glm::scale(kinect_model_matrix, glm::vec3(0.5f, 0.1f, 0.1f));
	GLUtils::defaultProgram->setUniform("model", kinect_model_matrix);
	Render::cube();

	const NUI_SKELETON_DATA *skeleton = app.getKinect().getFirstTrackedSkeletonData(skeletonFrame);
	if (nullptr != skeleton) {
		glm::mat4 skel_model_matrix;
		for (auto position : skeleton->SkeletonPositions) {
			glm::vec3 pos = glm::vec3(position.x, position.y, position.z);
			skel_model_matrix = glm::translate(glm::mat4(), pos);
			skel_model_matrix = glm::scale(skel_model_matrix, glm::vec3(0.1f,0.1f,0.1f));
			GLUtils::defaultProgram->setUniform("model", skel_model_matrix);
			Render::cube();
		}
	}

	glm::mat4 ground_model_matrix;
	ground_model_matrix = glm::translate(glm::mat4(), glm::vec3(0, -2, 0));//-skeletonFrame.vFloorClipPlane.w, 0));
	ground_model_matrix = glm::scale(ground_model_matrix, glm::vec3(5,1,5));
	GLUtils::defaultProgram->setUniform("model", ground_model_matrix);
	Render::ground();

	glm::mat4 cube_model_matrix;
	cube_model_matrix = glm::translate(glm::mat4(), glm::vec3(0, 0, -10));
	cube_model_matrix = glm::rotate(cube_model_matrix, d, glm::vec3(0,1,0));
	cube_model_matrix = glm::rotate(cube_model_matrix, d, glm::vec3(0,0,1));
	GLUtils::defaultProgram->setUniform("model", cube_model_matrix);
	Render::cube();
	d += 5.f;


	GLUtils::defaultProgram->stopUsing();

	window.display();
	window.popGLStates();
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

	const float threshold = 300.f; // central dead zone radius
	const float rate = 2.f;
	if (glm::length(mouse_pos_diff) > threshold) {
		camera.offsetOrientation(rate * mouse_pos_diff.y / threshold   // up angle offset
	                           , rate * mouse_pos_diff.x / threshold); // right angle offset
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
