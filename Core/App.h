#pragma once
#include <GL/glew.h>

#include "Kinect/KinectDevice.h"
#include "Messages/Messages.h"

#include "Windows/GLWindow.h"
#include "Windows/GUIWindow.h"

#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>


class App : public msg::Handler
{
public:
	App();
	~App();

	void run();

	sf::Time getDeltaTime() const;

	KinectDevice& getKinect();

	void process(const msg::QuitProgramMessage       *message);
	void process(const msg::StartKinectDeviceMessage *message);
	void process(const msg::StopKinectDeviceMessage  *message);

private:
	bool done;

	sf::Clock timer;

	KinectDevice kinect;

	GLWindow glWindow;
	GUIWindow guiWindow;

};


inline sf::Time App::getDeltaTime() const { return timer.getElapsedTime(); }

inline KinectDevice& App::getKinect() { return kinect; }
