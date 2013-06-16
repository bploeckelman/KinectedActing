#include "GLWindow.h"
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

static glm::mat4 projection_mat;
static glm::mat4 cube_modelview_mat;


GLWindow::GLWindow(const std::string& title)
	: Window(title)
{
	const sf::Uint32 style = sf::Style::Default;
	const sf::ContextSettings contextSettings(depth_bits, stencil_bits, antialias_level);
	videoMode = sf::VideoMode(sf::VideoMode::getDesktopMode().width  - 300
	                        , sf::VideoMode::getDesktopMode().height - 100, color_bits);
	window.create(videoMode, title, style, contextSettings);
	window.setFramerateLimit(framerate_limit);
	window.setPosition(sf::Vector2i(initial_pos_x, initial_pos_y));
}

GLWindow::~GLWindow()
{}

void GLWindow::init()
{
	const float windowWidth  = static_cast<float>(window.getSize().x);
	const float windowHeight = static_cast<float>(window.getSize().y);
	const float fovy         = 66.f;
	const float aspect       = windowWidth / windowHeight;
	const float znear        = 1.f;
	const float zfar         = 100.f;
	projection_mat = glm::perspective(fovy, aspect, znear, zfar);
	cube_modelview_mat  = glm::mat4(1.f);
}

void GLWindow::update()
{
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
			break;
		}
		if (event.type == sf::Event::MouseButtonPressed) {
			std::cout << "GL window clicked\n";
		}
	}
}

float d = 0.f; // temporary, for rotating cube
void GLWindow::render()
{
	window.setActive();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set projection matrix for default shader program
	glUseProgram(GLUtils::defaultProgram);
	glUniformMatrix4fv(GLUtils::projectionMatUniformLoc, 1, GL_FALSE, glm::value_ptr(projection_mat));

	// Render ground
	const glm::mat4 m = glm::translate(glm::mat4(), glm::vec3(0,-2,-10));
	glUniformMatrix4fv(GLUtils::modelviewMatUniformLoc,  1, GL_FALSE, glm::value_ptr(m));
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
}
