#include "GLWindow.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


GLWindow::GLWindow(const std::string& title)
	: Window(title)
{
	videoMode = sf::VideoMode(sf::VideoMode::getDesktopMode().width  - 300
	                        , sf::VideoMode::getDesktopMode().height - 100, 32);
	window.create(videoMode, title);
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(260, 5));
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
			std::cout << "GL click\n";
		}
	}
}

void GLWindow::render() {
	window.setActive();
	window.clear(sf::Color::Black);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(glm::perspective(66.f, static_cast<float>(window.getSize().y) / static_cast<float>(window.getSize().x), 0.1f, 100.f)));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(glm::mat4()));

	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5f, -0.5f, -10.f);
		glVertex3f( 0.5f, -0.5f, -10.f);
		glVertex3f(  0.f,  1.f,  -10.f);
	glEnd();

	window.display();
}
