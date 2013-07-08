#pragma once
#include <SFGUI/SFGUI.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Core/Windows/Window.h"


class GUI
{
public:
	GUI(Window& parentWindow);
	~GUI();

	void initialize();
	void update();
	void render();
	void handleEvent(const sf::Event& event);

public: // External interface
	void setKinectIdLabel(const std::string& text);
	void setRecordingLabel(const std::string& text);
	void setInfoLabel(const std::string& text);

	bool isLiveSkeletonVisible() const;

private:
	void layoutWidgets();
	void connectSignals();

	// Signal methods -----------------
	void onQuitButtonClick();
	void onStartKinectButtonClick();
	void onStopKinectButtonClick();
	void onRecordStartButtonClick();
	void onRecordStopButtonClick();
	void onRecordClearButtonClick();
	void onLiveSkeletonVisibleCheckButtonClick();
	void onPlaybackFirstButtonClick();
	void onPlaybackPreviousButtonClick();
	void onPlaybackStopButtonClick();
	void onPlaybackStartButtonClick();
	void onPlaybackNextButtonClick();
	void onPlaybackLastButtonClick();

private:
	Window& parentWindow;

	sfg::SFGUI sfgui;
	sfg::Desktop desktop;
	sfg::Window::Ptr window;

	sfg::Table::Ptr table;
	sfg::Button::Ptr quitButton;
	sfg::ScrolledWindow::Ptr kinectScrolledWindow;
	sfg::Box::Ptr kinectScrolledWindowBox;
	sfg::Label::Ptr kinectLabel;
	sfg::Label::Ptr kinectIdLabel;

	sfg::Button::Ptr startKinectButton;
	sfg::Button::Ptr stopKinectButton;

	sfg::CheckButton::Ptr liveSkeletonVisibleCheckButton;

	sfg::Label::Ptr  recordingLabel;
	sfg::Button::Ptr recordStartButton;
	sfg::Button::Ptr recordStopButton;
	sfg::Button::Ptr recordClearButton;

	sfg::Label::Ptr playbackLabel;
	sfg::ProgressBar::Ptr playbackProgressBar;
	sfg::Button::Ptr playbackFirstButton;
	sfg::Button::Ptr playbackPreviousButton;
	sfg::Button::Ptr playbackStopButton;
	sfg::Button::Ptr playbackStartButton;
	sfg::Button::Ptr playbackNextButton;
	sfg::Button::Ptr playbackLastButton;

	sfg::Label::Ptr infoLabel;

	bool liveSkeletonVisible;

};


inline bool GUI::isLiveSkeletonVisible() const { return liveSkeletonVisible; }
