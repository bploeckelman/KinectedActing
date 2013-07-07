#pragma once
#include "Window.h"
#include "Scene/Camera.h"

#include <SFML/System/Time.hpp>

#include <string>
#include <memory>

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
	void handleEvents();
	void updateCamera();
	void updateRecording();
	void updateTextures();

	sf::Time animTimer;

	tdogl::Camera camera;

	std::shared_ptr<tdogl::Texture> colorTexture;
	std::shared_ptr<tdogl::Texture> depthTexture;
	std::shared_ptr<tdogl::Texture> gridTexture;

	std::shared_ptr<Animation> animation;
	std::shared_ptr<Skeleton>  skeleton;

};
