#include "GLWindow.h"
#include "Util/RenderUtils.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

const int color_bits      = 32;
const int depth_bits      = 24;
const int stencil_bits    = 8;
const int antialias_level = 2;
const int framerate_limit = 60;
const int initial_pos_x   = 260;
const int initial_pos_y   = 5;

glm::mat4 projection_mat;
glm::mat4 modelview_mat;


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

	const float windowWidth  = static_cast<float>(window.getSize().x);
	const float windowHeight = static_cast<float>(window.getSize().y);
	const float fovy         = 66.f;
	const float aspect       = windowWidth / windowHeight;
	const float znear        = 1.f;
	const float zfar         = 100.f;
	projection_mat = glm::perspective(fovy, aspect, znear, zfar);
	modelview_mat  = glm::mat4(1.f);
}

GLWindow::~GLWindow()
{}

void GLWindow::update() {
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
void GLWindow::render() {
	window.setActive();
	window.clear(sf::Color::Black);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(projection_mat));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(modelview_mat));

	glPushMatrix();
	glTranslatef(0, -2.f, -10.f);
	glColor3f(1,0,0);
	Render::ground();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -20.f);
	glRotatef(d, 0, 1, 0);
	glRotatef(d, 0, 0, 1);
	glColor3f(0,1,0);
	Render::cube();
	glPopMatrix();
	d += 1.f;

	window.display();
}
