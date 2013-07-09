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
	GLWindow& getGLWindow();
	GUIWindow& getGUIWindow();

	bool isRecording() const { return recording; }

	void process(const msg::QuitProgramMessage       *message);
	void process(const msg::StartKinectDeviceMessage *message);
	void process(const msg::StopKinectDeviceMessage  *message);
	void process(const msg::StartRecordingMessage    *message);
	void process(const msg::StopRecordingMessage     *message);

private:
	bool done;
	bool recording;

	sf::Clock timer;

	KinectDevice kinect;

	GLWindow glWindow;
	GUIWindow guiWindow;

};


inline sf::Time App::getDeltaTime() const { return timer.getElapsedTime(); }

inline KinectDevice& App::getKinect() { return kinect; }
inline GLWindow& App::getGLWindow() { return glWindow; }
inline GUIWindow& App::getGUIWindow() { return guiWindow; }
