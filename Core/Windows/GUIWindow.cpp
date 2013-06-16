#include "GUIWindow.h"

#include <iostream>


GUIWindow::GUIWindow(const std::string& title)
	: Window(title)
	, gui(window)
{
	videoMode = sf::VideoMode(256, sf::VideoMode::getDesktopMode().height - 50, 32);
	window.create(videoMode, title, sf::Style::None);
	window.setFramerateLimit(30);
	window.setPosition(sf::Vector2i(2, 5));

	gui.initialize();
}

GUIWindow::~GUIWindow()
{}


void GUIWindow::update() {
	sf::Event event;
	while (window.pollEvent(event)) {
		gui.handleEvent(event);
	}
	gui.update();
}

void GUIWindow::render() {
	window.setActive();
	window.clear(sf::Color::Black);
	gui.render();
	window.display();
}
