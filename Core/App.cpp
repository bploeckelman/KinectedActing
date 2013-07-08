#include "App.h"
#include "Util/GLUtils.h"
#include "Util/RenderUtils.h"
#include "Messages/Messages.h"

#include <iostream>


App::App()
	: done(false)
	, recording(false)
	, timer()
	, kinect()
	, guiWindow("GUI", *this)
	, glWindow("OpenGL Window", *this)
{
	GLUtils::init();
	Render::init();

	guiWindow.init();
	glWindow.init();

	gMessageDispatcher.registerHandler(Message::QUIT_PROGRAM, this);
	gMessageDispatcher.registerHandler(Message::START_KINECT_DEVICE, this);
	gMessageDispatcher.registerHandler(Message::STOP_KINECT_DEVICE, this);
	gMessageDispatcher.registerHandler(Message::START_SKELETON_RECORDING, this);
	gMessageDispatcher.registerHandler(Message::STOP_SKELETON_RECORDING, this);
}

App::~App()
{
	GLUtils::cleanup();
	Render::cleanup();
}

void App::run()
{
	while (!done) {
		if (kinect.isInitialized()) {
			kinect.update();
		}

		guiWindow.update();
		glWindow.update();

		if (!guiWindow.getWindow().isOpen() || !glWindow.getWindow().isOpen()) {
			done = true;
			break;
		}

		guiWindow.render();
		glWindow.render();

		timer.restart();
	}
}

void App::process( const QuitProgramMessage* message )
{
	guiWindow.getWindow().close();
	glWindow.getWindow().close();
}

void App::process( const StartKinectDeviceMessage* message )
{
	kinect.init();
	guiWindow.getGUI().setKinectIdLabel(kinect.getDeviceId());
}

void App::process( const StopKinectDeviceMessage* message )
{
	kinect.shutdown();
	guiWindow.getGUI().setKinectIdLabel("[ offline ]");
}

void App::process( const StartRecordingMessage *message )
{
	recording = true;
}

void App::process( const StopRecordingMessage *message )
{
	recording = false;
}
