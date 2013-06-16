#pragma once
#include "Window.h"
#include "Core/GUI/UserInterface.h"

#include <string>


class GUIWindow : public Window
{
public:
	GUIWindow(const std::string& title);
	virtual ~GUIWindow();

	void update();
	void render();

private:
	GUI gui;
	
};
