#include "App.h"
#include "Util/RenderUtils.h"

#include <iostream>


App::App()
	: done(false)
	, guiWindow("GUI")
	, glWindow("OpenGL Window")
{
	// Initialize glew
	GLenum result = glewInit();
	if (GLEW_OK == result) {
		std::cout << "GLEW initialized: " << glewGetString(GLEW_VERSION) << "\n";
	} else {
		std::cerr << "Failed to initialize glew.\n"
	              << "Reason: " << glewGetErrorString(result) << "\n";
	}

	// Initialize render utilities
	Render::init();
}

App::~App()
{
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
