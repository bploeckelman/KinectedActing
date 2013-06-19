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

public: // External interface
	void setKinectIdLabel(const std::string& label);

private:
	void layoutWidgets();
	void connectSignals();

	// Signal methods -----------------
	void onQuitButtonClick();

private:
	sf::RenderWindow& renderWindow;

	sfg::SFGUI sfgui;
	sfg::Desktop desktop;
	sfg::Window::Ptr window;

	sfg::Box::Ptr box;
	sfg::Button::Ptr quitButton;

	sfg::Table::Ptr kinectTable;
	sfg::ScrolledWindow::Ptr kinectScrolledWindow;
	sfg::Box::Ptr kinectScrolledWindowBox;
	sfg::Label::Ptr kinectIdLabel;

};
