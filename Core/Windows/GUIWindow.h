#pragma once
#include "Window.h"

#include <SFGUI/SFGUI.hpp>

#include <string>


class GUIWindow : public Window
{
public:
	GUIWindow(const std::string& title);
	virtual ~GUIWindow();

	void update();
	void render();

private:
	// TODO : extract to a gui class
	void onButtonClick();

private:
	// TODO : extract to a gui class
	sfg::SFGUI sfgui;
	sfg::Desktop desktop;
	sfg::Box::Ptr box;
	sfg::Fixed::Ptr fixed;
	sfg::Window::Ptr gui;
	sfg::Button::Ptr button;
	
};
