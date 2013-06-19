#pragma once
#include "Window.h"

#include <string>


class GLWindow : public Window
{
public:
	GLWindow(const std::string& title, App& app);
	virtual ~GLWindow();

	void init();
	void update();
	void render();

};
