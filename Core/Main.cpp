#include "Windows/GLWindow.h"
#include "Windows/GUIWindow.h"

bool quit;


int main()
{
	GLWindow glWindow("OpenGL Window");
	GUIWindow guiWindow("GUI");

	quit = false;
	while (!quit) {
		guiWindow.update();
		guiWindow.render();

		glWindow.update();
		glWindow.render();
	}

	return 0;
}
