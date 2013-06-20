#include "GLWindow.h"
#include "Core/App.h"
#include "Kinect/KinectDevice.h"
#include "Scene/Camera.h"
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
static const int antialias_level = 2;
static const int framerate_limit = 60;
static const int initial_pos_x   = 260;
static const int initial_pos_y   = 5;

static glm::mat4 cube_modelview_mat;
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
	cube_modelview_mat  = glm::mat4(1.f);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // because swank

	// Set projection matrix for default shader program
	glUseProgram(GLUtils::defaultProgram);

	// Combined projection + camera transformation matrix
	const glm::mat4& camera_mat(camera.matrix());
	glUniformMatrix4fv(GLUtils::projectionMatUniformLoc, 1, GL_FALSE, glm::value_ptr(camera_mat));

	// Render ground
	const glm::mat4 ground_mat = glm::translate(glm::mat4(), glm::vec3(0,-2,-10));
	glUniformMatrix4fv(GLUtils::modelviewMatUniformLoc,  1, GL_FALSE, glm::value_ptr(ground_mat));
	Render::ground();

	// Render cube
	cube_modelview_mat = glm::translate(glm::mat4(), glm::vec3(0,0,-20));
	cube_modelview_mat = glm::rotate(cube_modelview_mat, d, glm::vec3(0,1,0));
	cube_modelview_mat = glm::rotate(cube_modelview_mat, d, glm::vec3(0,0,1));
	glUniformMatrix4fv(GLUtils::modelviewMatUniformLoc,  1, GL_FALSE, glm::value_ptr(cube_modelview_mat));
	Render::cube();

	// Update rotation amount
	d += 1.f;

	glUseProgram(0);

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
