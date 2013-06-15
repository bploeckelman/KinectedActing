#include "GLWindow.h"

#include <SFML/Window/Event.hpp>

#include <iostream>

extern bool quit;


GLWindow::GLWindow(const std::string& title)
	: Window(title)
{
	videoMode = sf::VideoMode(sf::VideoMode::getDesktopMode().width  - 256 - 64 
	                        , sf::VideoMode::getDesktopMode().height - 128, 32);
	window.create(videoMode, title);
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(256 + 32, 10));
}

GLWindow::~GLWindow()
{}

void GLWindow::update() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			quit = true;
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
	// Draw some things
	window.display();
}
