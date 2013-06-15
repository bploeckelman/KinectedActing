#include "GUIWindow.h"

#include <iostream>

extern bool quit;

GUIWindow::GUIWindow(const std::string& title)
	: Window(title)
	, sfgui()
	, desktop()
	, box(sfg::Box::Create(sfg::Box::VERTICAL))
	, fixed(sfg::Fixed::Create())
	, gui(sfg::Window::Create())
	, button(sfg::Button::Create("Quit"))
{
	videoMode = sf::VideoMode(256, sf::VideoMode::getDesktopMode().height - 128, 32);
	window.create(videoMode, title);
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(10, 10));

	button->GetSignal(sfg::Button::OnLeftClick).Connect(&GUIWindow::onButtonClick, this);

	fixed->Put(button, sf::Vector2f(0, 0));
	box->Pack(fixed);
	gui->SetTitle("Kinected Acting");
	gui->SetStyle(sfg::Window::Style::NO_STYLE);
	gui->SetRequisition(sf::Vector2f(window.getSize()));
	gui->Add(box);
	desktop.Add(gui);
}

GUIWindow::~GUIWindow()
{}


void GUIWindow::update() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			quit = true;
			break;
		}
		if (event.type == sf::Event::MouseButtonPressed) {
			std::cout << "GUI click\n";
		}
		desktop.HandleEvent(event);
	}
	desktop.Update(1.f);
}

void GUIWindow::render() {
	window.setActive();
	window.clear(sf::Color::Black);
	window.pushGLStates();
	sfgui.Display(window);
	window.popGLStates();
	window.display();
}

void GUIWindow::onButtonClick() {
	std::cout << "GUI Button clicked\n";
	quit = true;
}
