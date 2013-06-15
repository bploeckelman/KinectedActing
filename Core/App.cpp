#include "App.h"


App::App()
	: done(false)
	, guiWindow("GUI")
	, glWindow("OpenGL Window")
{}

App::~App()
{}

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
