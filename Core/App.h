#pragma once
#include <GL/glew.h>

#include "Kinect/KinectDevice.h"

#include "Windows/GLWindow.h"
#include "Windows/GUIWindow.h"


class App
{
public:
	App();
	~App();

	void run();

	KinectDevice& getKinect();
	GLWindow& getGLWindow();
	GUIWindow& getGUIWindow();

private:
	bool done;

	KinectDevice kinect;

	GLWindow glWindow;
	GUIWindow guiWindow;

};


inline KinectDevice& App::getKinect() { return kinect; }
inline GLWindow& App::getGLWindow() { return glWindow; }
inline GUIWindow& App::getGUIWindow() { return guiWindow; }
