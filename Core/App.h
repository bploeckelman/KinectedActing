#pragma once
#include <GL/glew.h>

#include "Windows/GLWindow.h"
#include "Windows/GUIWindow.h"


class App
{
public:
	App();
	~App();

	void run();

private:
	bool done;

	GLWindow glWindow;
	GUIWindow guiWindow;

};
