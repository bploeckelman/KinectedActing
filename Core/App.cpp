#include "App.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"

#include <iostream>


App::App()
	: done(false)
	, guiWindow("GUI")
	, glWindow("OpenGL Window")
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
		guiWindow.update();
		glWindow.update();

		guiWindow.render();
		glWindow.render();

		if (!guiWindow.getWindow().isOpen() || !glWindow.getWindow().isOpen()) {
			done = true;
		}
	}
}
