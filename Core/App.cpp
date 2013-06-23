#include "App.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"

#include <iostream>


App::App()
	: done(false)
	, kinect()
	, guiWindow("GUI", *this)
	, glWindow("OpenGL Window", *this)
{
	GLUtils::init();
	Render::init();

	kinect.init();

	guiWindow.init();
	glWindow.init();
}

App::~App()
{
	GLUtils::cleanup();
	Render::cleanup();
}

void App::run()
{
	while (!done) {
		kinect.update();

		guiWindow.update();
		glWindow.update();

		guiWindow.render();
		glWindow.render();

		if (!guiWindow.getWindow().isOpen() || !glWindow.getWindow().isOpen()) {
			done = true;
		}
	}
}
