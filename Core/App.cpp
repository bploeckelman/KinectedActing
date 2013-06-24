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
		if (kinect.isInitialized()) {
			kinect.update();
		}

		guiWindow.update();
		glWindow.update();

		if (!guiWindow.getWindow().isOpen() || !glWindow.getWindow().isOpen()) {
			done = true;
			break;
		}

		guiWindow.render();
		glWindow.render();
	}
}
