#pragma once
#include <SFGUI/SFGUI.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


class GUI
{
public:
	GUI(sf::RenderWindow& renderWindow);
	~GUI();

	void initialize();
	void update();
	void render();
	void handleEvent(const sf::Event& event);

private:
	void layoutWidgets();
	void connectSignals();

	// Signal methods -----------------
	void onButtonClick();

private:
	sf::RenderWindow& renderWindow;

	sfg::SFGUI sfgui;
	sfg::Desktop desktop;
	sfg::Window::Ptr window;

	sfg::Fixed::Ptr fixed;
	sfg::Button::Ptr button;

};
