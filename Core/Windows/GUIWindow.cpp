#include "Core/App.h"
#include "GUIWindow.h"
#include "Core/Messages/Messages.h"

#include <iostream>

static const int window_width    = 256;
static const int height_offset   = 50;
static const int color_depth     = 32;
static const int framerate_limit = 30;
static const int initial_pos_x   = 2;
static const int initial_pos_y   = 5;
static const sf::Uint32 style    = sf::Style::None;


GUIWindow::GUIWindow(const std::string& title, App& app)
	: Window(title, app)
	, gui()
{
	videoMode = sf::VideoMode(window_width
	                        , sf::VideoMode::getDesktopMode().height - height_offset
							, color_depth);
	window.create(videoMode, title, style);
	window.setFramerateLimit(framerate_limit);
	window.setPosition(sf::Vector2i(initial_pos_x, initial_pos_y));

	registerMessageHandlers();
}

GUIWindow::~GUIWindow()
{}

void GUIWindow::init()
{
	gui.initialize(window);
	gui.setKinectIdLabel(app.getKinect().getDeviceId());
}

void GUIWindow::update()
{
	sf::Event event;
	while (window.pollEvent(event)) {
		gui.handleEvent(event);
	}
	gui.update();
}

void GUIWindow::render()
{
	window.setActive();
	window.clear(sf::Color::Black);
	gui.render(window);
	window.display();
}


// ----------------------------------------------------------------------------
// Message processing methods -------------------------------------------------
// ----------------------------------------------------------------------------
void GUIWindow::registerMessageHandlers()
{
	msg::gDispatcher.registerHandler(msg::SET_RECORDING_LABEL,   this);
	msg::gDispatcher.registerHandler(msg::PLAYBACK_SET_PROGRESS, this);
	msg::gDispatcher.registerHandler(msg::SET_INFO_LABEL,        this);
	msg::gDispatcher.registerHandler(msg::ADD_LAYER_ITEM,        this);
}

void GUIWindow::process( const msg::SetRecordingLabelMessage *message )
{
	gui.setRecordingLabel(message->text);
}

void GUIWindow::process( const msg::PlaybackSetProgressMessage *message )
{
	gui.setProgressFraction(message->progress);
}

void GUIWindow::process( const msg::SetInfoLabelMessage *message )
{
	gui.setInfoLabel(message->text);
}

void GUIWindow::process( const msg::AddLayerItemMessage *message )
{
	gui.appendLayerItem(message->item);
}
