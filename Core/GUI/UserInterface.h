#pragma once
#include <SFGUI/SFGUI.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


class GUI
{
public:
	GUI();
	~GUI();

	void initialize(sf::RenderWindow& parentWindow);
	void update();
	void render(sf::RenderWindow& parentWindow);
	void handleEvent(const sf::Event& event);

public: // External interface
	void setKinectIdLabel(const std::string& text);
	void setRecordingLabel(const std::string& text);
	void setInfoLabel(const std::string& text);
	void setProgressFraction(const float fraction);

	bool isLiveSkeletonVisible() const;

private:
	void layoutWidgets(sf::RenderWindow& parentWindow);
	void connectSignals();

	// Signal methods -----------------
	void onQuitButtonClick();
	void onStartKinectButtonClick();
	void onStopKinectButtonClick();
	void onRecordStartButtonClick();
	void onRecordStopButtonClick();
	void onRecordClearButtonClick();
	void onLiveSkeletonVisibleCheckButtonClick();
	void onPlaybackProgressBarClick();
	void onPlaybackFirstButtonClick();
	void onPlaybackPreviousButtonClick();
	void onPlaybackStopButtonClick();
	void onPlaybackStartButtonClick();
	void onPlaybackNextButtonClick();
	void onPlaybackLastButtonClick();
	void onPlaybackDeltaScaleClick();
	void onStartLayeringButtonClick();
	void onAnimLayersComboBoxSelect();

private:
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
	sfg::Scale::Ptr playbackDeltaScale;

	sfg::Button::Ptr startLayeringButton;
	sfg::ComboBox::Ptr animLayersComboBox;

	sfg::Label::Ptr infoLabel;

};
