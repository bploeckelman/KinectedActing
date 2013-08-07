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
	void appendLayerItem(const std::string& text);

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
	void onRecordExportButtonClick();
	void onSeatedModeEnabledButtonClick();
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
	void onMappingModeComboBoxSelect();
	void onFilteringLevelComboBoxSelect();
	void onRenderPathCheckButtonClick();
	void onBoneMaskToggleButtonClick();

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

	sfg::Button::Ptr seatedModeEnabledButton;
	sfg::CheckButton::Ptr liveSkeletonVisibleCheckButton;

	sfg::Label::Ptr  recordingLabel;
	sfg::Button::Ptr recordStartButton;
	sfg::Button::Ptr recordStopButton;
	sfg::Button::Ptr recordClearButton;
	sfg::Button::Ptr recordExportButton;

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
	sfg::ComboBox::Ptr mappingModesComboBox;
	sfg::ComboBox::Ptr filterLevelsComboBox;

	sfg::CheckButton::Ptr renderPathCheckButton;

	// TODO : add check/toggle buttons for all joints
	sfg::ToggleButton::Ptr headToggleButton;
	sfg::ToggleButton::Ptr shoulderCenterToggleButton;
	sfg::ToggleButton::Ptr spineToggleButton;
	sfg::ToggleButton::Ptr hipCenterToggleButton;

	sfg::ToggleButton::Ptr shoulderLeftToggleButton;
	sfg::ToggleButton::Ptr elbowLeftToggleButton;
	sfg::ToggleButton::Ptr wristLeftToggleButton;
	sfg::ToggleButton::Ptr handLeftToggleButton;

	sfg::ToggleButton::Ptr shoulderRightToggleButton;
	sfg::ToggleButton::Ptr elbowRightToggleButton;
	sfg::ToggleButton::Ptr wristRightToggleButton;
	sfg::ToggleButton::Ptr handRightToggleButton;

	sfg::ToggleButton::Ptr hipLeftToggleButton;
	sfg::ToggleButton::Ptr kneeLeftToggleButton;
	sfg::ToggleButton::Ptr ankleLeftToggleButton;
	sfg::ToggleButton::Ptr footLeftToggleButton;

	sfg::ToggleButton::Ptr hipRightToggleButton;
	sfg::ToggleButton::Ptr kneeRightToggleButton;
	sfg::ToggleButton::Ptr ankleRightToggleButton;
	sfg::ToggleButton::Ptr footRightToggleButton;

	sfg::Label::Ptr infoLabel;

};
