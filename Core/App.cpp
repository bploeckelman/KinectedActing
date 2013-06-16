#include "App.h"
#include "Util/GLUtils.h"
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
		if (!glewIsSupported("GL_VERSION_3_3")) {
			std::cerr << "OpenGL 3.0 is not supported\n";
			exit(1);
		}
	} else {
		std::cerr << "Failed to initialize glew.\n"
	              << "Reason: " << glewGetErrorString(result) << "\n";
		exit(1);
	}

	// Initialize OpenGL utilities
	GLUtils::init();

	// Initialize render utilities
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
