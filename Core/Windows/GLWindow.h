#pragma once
#include "Window.h"
#include "Scene/Camera.h"
#include "Util/GLUtils.h"

#include <string>

namespace tdogl { class Texture; }

class Animation;
class Skeleton;


class GLWindow : public Window
{
public:
	GLWindow(const std::string& title, App& app);
	virtual ~GLWindow();

	void init();
	void update();
	void render();

private:
	void resetCamera();
	void updateCamera();
	void updateRecording();

	tdogl::Camera camera;

	tdogl::Texture *colorTexture;
	tdogl::Texture *depthTexture;

	Animation *animation;
	Skeleton *skeleton;

};
