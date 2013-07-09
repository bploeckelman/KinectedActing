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

	msg::gDispatcher.registerHandler(msg::QUIT_PROGRAM,             this);
	msg::gDispatcher.registerHandler(msg::START_KINECT_DEVICE,      this);
	msg::gDispatcher.registerHandler(msg::STOP_KINECT_DEVICE,       this);
	msg::gDispatcher.registerHandler(msg::START_SKELETON_RECORDING, this);
	msg::gDispatcher.registerHandler(msg::STOP_SKELETON_RECORDING,  this);
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

void App::process( const msg::QuitProgramMessage* message )
{
	guiWindow.getWindow().close();
	glWindow.getWindow().close();
}

void App::process( const msg::StartKinectDeviceMessage* message )
{
	kinect.init();
	guiWindow.getGUI().setKinectIdLabel(kinect.getDeviceId());
}

void App::process( const msg::StopKinectDeviceMessage* message )
{
	kinect.shutdown();
	guiWindow.getGUI().setKinectIdLabel("[ offline ]");
}

void App::process( const msg::StartRecordingMessage *message )
{
	recording = true;
}

void App::process( const msg::StopRecordingMessage *message )
{
	recording = false;
}
