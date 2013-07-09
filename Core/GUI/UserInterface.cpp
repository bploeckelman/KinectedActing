#include <GL/glew.h>

#include "Kinect/KinectDevice.h"

#include "UserInterface.h"
#include "Core/Messages/Messages.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <string>


GUI::GUI()
	: sfgui()
	, desktop()
	, window(sfg::Window::Create())
	, quitButton(sfg::Button::Create("Quit"))
	, table(sfg::Table::Create())
	, kinectScrolledWindow(sfg::ScrolledWindow::Create())
	, kinectScrolledWindowBox(sfg::Box::Create(sfg::Box::HORIZONTAL))
	, kinectLabel(sfg::Label::Create("Kinect Sensor:"))
	, kinectIdLabel(sfg::Label::Create())
	, startKinectButton(sfg::Button::Create("Start Kinect"))
	, stopKinectButton(sfg::Button::Create("Stop Kinect"))
	, recordingLabel(sfg::Label::Create("Skeleton Recording:"))
	, recordStartButton(sfg::Button::Create("Start Recording"))
	, recordStopButton(sfg::Button::Create("Stop Recording"))
	, recordClearButton(sfg::Button::Create("Clear Recorded Keyframes"))
	, playbackLabel(sfg::Label::Create("Playback Controls:"))
	, playbackProgressBar(sfg::ProgressBar::Create())
	, playbackFirstButton(sfg::Button::Create("<<"))
	, playbackPreviousButton(sfg::Button::Create("<"))
	, playbackStopButton(sfg::Button::Create("Stop"))
	, playbackStartButton(sfg::Button::Create("Start"))
	, playbackNextButton(sfg::Button::Create(">"))
	, playbackLastButton(sfg::Button::Create(">>"))
	, playbackDeltaScale(sfg::Scale::Create(1.f / 1000.f, 1.f, 1.f / 1000.f))
	, infoLabel(sfg::Label::Create(""))
	, liveSkeletonVisibleCheckButton(sfg::CheckButton::Create("Show Live Skeleton"))
{}

GUI::~GUI()
{}

void GUI::update()
{
	desktop.Update(1.f);
}

void GUI::render( sf::RenderWindow& parentWindow )
{
	parentWindow.pushGLStates();
	sfgui.Display(parentWindow);
	parentWindow.popGLStates();
}

void GUI::initialize( sf::RenderWindow& parentWindow )
{
	layoutWidgets(parentWindow);
	connectSignals();
}

void GUI::handleEvent(const sf::Event& event)
{
	desktop.HandleEvent(event);
}

void GUI::layoutWidgets( sf::RenderWindow& parentWindow )
{
	const sf::Vector2f winsize(parentWindow.getSize());

	kinectLabel->SetText("Kinect Sensor:");
	kinectLabel->SetAlignment(sf::Vector2f(0.f,0.75f));

	kinectIdLabel->SetText("[ offline ]");
	kinectScrolledWindowBox->Pack(kinectIdLabel);
	kinectScrolledWindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_ALWAYS | sfg::ScrolledWindow::VERTICAL_NEVER);
	kinectScrolledWindow->SetHorizontalAdjustment(sfg::Adjustment::Create(0.f, 0.f, 1.f, 0.1f, 0.5f, 0.5f));
	kinectScrolledWindow->AddWithViewport(kinectScrolledWindowBox);

	playbackDeltaScale->SetValue(1.f / 60.f);
	liveSkeletonVisibleCheckButton->SetActive(true);

	const sf::Uint32 colspan = 6;
	table->SetColumnSpacings(1.f);

	// Attach widget:  (widgetPtr, <col idx, row idx, col span, row span>, horizontal packing, vertical packing, padding)
	table->Attach(quitButton, sf::Rect<sf::Uint32>(0, 0, colspan, 1), sfg::Table::FILL, sfg::Table::FILL);

	table->Attach(kinectLabel,          sf::Rect<sf::Uint32>(0, 1, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	table->Attach(kinectScrolledWindow, sf::Rect<sf::Uint32>(0, 2, colspan, 1), sfg::Table::FILL, sfg::Table::FILL);

	sfg::Label::Ptr deviceLabel = sfg::Label::Create("Kinect Device:");
	deviceLabel->SetAlignment(sf::Vector2f(0.f, 0.75f));
	table->Attach(deviceLabel,       sf::Rect<sf::Uint32>(0, 3, colspan,     1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	table->Attach(startKinectButton, sf::Rect<sf::Uint32>(0, 4, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(stopKinectButton,  sf::Rect<sf::Uint32>(3, 4, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);

	table->Attach(liveSkeletonVisibleCheckButton, sf::Rect<sf::Uint32>(0, 5, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));

	recordingLabel->SetText("Skeleton Recording:");
	recordingLabel->SetAlignment(sf::Vector2f(0.f, 0.75f));
	table->Attach(recordingLabel,    sf::Rect<sf::Uint32>(0, 6, colspan,     1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	table->Attach(recordStartButton, sf::Rect<sf::Uint32>(0, 7, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(recordStopButton,  sf::Rect<sf::Uint32>(3, 7, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->SetRowSpacing(7, 2.5f);
	table->Attach(recordClearButton, sf::Rect<sf::Uint32>(0, 8, colspan,     1), sfg::Table::FILL, sfg::Table::FILL);

	playbackLabel->SetAlignment(sf::Vector2f(0.f, 0.75f));
	table->Attach(playbackLabel,       sf::Rect<sf::Uint32>(0,  9, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.));
	table->Attach(playbackProgressBar, sf::Rect<sf::Uint32>(0, 10, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 10.f));

	table->SetRowSpacing(10, 5.f);
	table->Attach(playbackFirstButton,    sf::Rect<sf::Uint32>(0, 11, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackPreviousButton, sf::Rect<sf::Uint32>(1, 11, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackStopButton,     sf::Rect<sf::Uint32>(2, 11, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackStartButton,    sf::Rect<sf::Uint32>(3, 11, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackNextButton,     sf::Rect<sf::Uint32>(4, 11, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackLastButton,     sf::Rect<sf::Uint32>(5, 11, 1, 1), sfg::Table::FILL, sfg::Table::FILL);

	table->SetRowSpacing(11, 5.f);
	sfg::Label::Ptr deltaScaleLabel(sfg::Label::Create("Delta"));
	table->Attach(deltaScaleLabel,    sf::Rect<sf::Uint32>(0, 12,           2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 2.f));
	table->Attach(playbackDeltaScale, sf::Rect<sf::Uint32>(2, 12, colspan - 2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 2.f));

	infoLabel->SetAlignment(sf::Vector2f(0.f, 0.5f));
	table->Attach(infoLabel, sf::Rect<sf::Uint32>(0, 13, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 10.f));

	window->SetTitle("Kinected Acting");
	window->SetRequisition(winsize);
	window->Add(table);

	desktop.Add(window);
}

void GUI::connectSignals()
{
	quitButton       ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onQuitButtonClick,        this);

	startKinectButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onStartKinectButtonClick, this);
	stopKinectButton ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onStopKinectButtonClick,  this);

	recordStartButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onRecordStartButtonClick, this);
	recordStopButton ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onRecordStopButtonClick,  this);
	recordClearButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onRecordClearButtonClick, this);

	liveSkeletonVisibleCheckButton->GetSignal(sfg::CheckButton::OnLeftClick).Connect(&GUI::onLiveSkeletonVisibleCheckButtonClick, this);

	//playbackProgressBar->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onButtonClick, this);
	playbackFirstButton   ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackFirstButtonClick,    this);
	playbackPreviousButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackPreviousButtonClick, this);
	playbackStopButton    ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackStopButtonClick,     this);
	playbackStartButton   ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackStartButtonClick,    this);
	playbackNextButton    ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackNextButtonClick,     this);
	playbackLastButton    ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackLastButtonClick,     this);
	playbackDeltaScale    ->GetSignal(sfg::Scale::OnLeftClick ).Connect(&GUI::onPlaybackDeltaScaleClick,     this);
}

void GUI::onQuitButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::QuitProgramMessage());
}

void GUI::onStartKinectButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::StartKinectDeviceMessage());
}

void GUI::onStopKinectButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::StopKinectDeviceMessage());
}

void GUI::setKinectIdLabel(const std::string& text)
{
	kinectIdLabel->SetText(text);
}

void GUI::setRecordingLabel(const std::string& text)
{
	recordingLabel->SetText(text);
}

void GUI::setInfoLabel(const std::string& text)
{
	infoLabel->SetText(text);
}

void GUI::onRecordStartButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::StartRecordingMessage());
}

void GUI::onRecordStopButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::StopRecordingMessage());
}

void GUI::onRecordClearButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::ClearRecordingMessage());
}

void GUI::onLiveSkeletonVisibleCheckButtonClick()
{
	const bool active = liveSkeletonVisibleCheckButton->IsActive();
	if (active) {
		msg::gDispatcher.dispatchMessage(msg::ShowLiveSkeletonMessage());
	} else {
		msg::gDispatcher.dispatchMessage(msg::HideLiveSkeletonMessage());
	}
}

void GUI::onPlaybackFirstButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::PlaybackFirstFrameMessage());
}

void GUI::onPlaybackPreviousButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::PlaybackPrevFrameMessage());
}

void GUI::onPlaybackStopButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::PlaybackStopMessage());
}

void GUI::onPlaybackStartButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::PlaybackStartMessage());
}

void GUI::onPlaybackNextButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::PlaybackNextFrameMessage());
}

void GUI::onPlaybackLastButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::PlaybackLastFrameMessage());
}

void GUI::onPlaybackDeltaScaleClick()
{
	msg::gDispatcher.dispatchMessage(msg::PlaybackSetDeltaMessage(playbackDeltaScale->GetValue()));
}
