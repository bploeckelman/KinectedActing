#pragma once
#include <GL/glew.h>

#include "Kinect/KinectDevice.h"

#include "Windows/GLWindow.h"
#include "Windows/GUIWindow.h"

#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>


class App
{
public:
	App();
	~App();

	void run();

	sf::Time getDeltaTime() const;

	KinectDevice& getKinect();
	GLWindow& getGLWindow();
	GUIWindow& getGUIWindow();

private:
	bool done;

	sf::Clock timer;

	KinectDevice kinect;

	GLWindow glWindow;
	GUIWindow guiWindow;

};


inline sf::Time App::getDeltaTime() const { return timer.getElapsedTime(); }

inline KinectDevice& App::getKinect() { return kinect; }
inline GLWindow& App::getGLWindow() { return glWindow; }
inline GUIWindow& App::getGUIWindow() { return guiWindow; }
