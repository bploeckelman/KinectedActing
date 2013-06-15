#pragma once
#include "Window.h"

#include <string>


class GLWindow : public Window
{
public:
	GLWindow(const std::string& title);
	virtual ~GLWindow();

	void update();
	void render();
	
};