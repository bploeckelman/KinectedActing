#include "GUIWindow.h"

#include <iostream>

const int window_width    = 256;
const int height_offset   = 50;
const int color_depth     = 32;
const int framerate_limit = 30;
const int initial_pos_x   = 2;
const int initial_pos_y   = 5;
const sf::Uint32 style    = sf::Style::None;


GUIWindow::GUIWindow(const std::string& title)
	: Window(title)
	, gui(window)
{
	videoMode = sf::VideoMode(window_width
	                        , sf::VideoMode::getDesktopMode().height - height_offset
							, color_depth);
	window.create(videoMode, title, style);
	window.setFramerateLimit(framerate_limit);
	window.setPosition(sf::Vector2i(initial_pos_x, initial_pos_y));

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
