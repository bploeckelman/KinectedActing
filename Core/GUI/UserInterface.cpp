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
	, playbackDeltaScale(sfg::Scale::Create(0.00000111f, 0.1f, 0.0000222222f))
	, startLayeringButton(sfg::Button::Create("Create New Layer"))
	, animLayersComboBox(sfg::ComboBox::Create())
	, infoLabel(sfg::Label::Create(""))
	, liveSkeletonVisibleCheckButton(sfg::CheckButton::Create("Show Live Skeleton"))
	, renderPathCheckButton(sfg::CheckButton::Create("Draw bone path"))
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
	renderPathCheckButton->SetActive(false);

	animLayersComboBox->AppendItem("base");
	animLayersComboBox->AppendItem("blend");
	animLayersComboBox->SelectItem(0);

	const sf::Uint32 colspan = 6;
	table->SetColumnSpacings(2.f);

	// Attach widget:  (widgetPtr, <col idx, row idx, col span, row span>, horizontal packing, vertical packing, padding)
	table->Attach(quitButton, sf::Rect<sf::Uint32>(0, 0, colspan, 1), sfg::Table::FILL, sfg::Table::FILL);

	table->SetRowSpacing(0, 10.f);
	table->Attach(kinectLabel,          sf::Rect<sf::Uint32>(0, 1, colspan    , 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	table->Attach(kinectScrolledWindow, sf::Rect<sf::Uint32>(0, 2, colspan    , 1), sfg::Table::FILL, sfg::Table::FILL);
	table->SetRowSpacing(2, 5.f);
	table->Attach(startKinectButton,    sf::Rect<sf::Uint32>(0, 3, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(stopKinectButton,     sf::Rect<sf::Uint32>(3, 3, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->SetRowSpacing(3, 5.f);
	table->Attach(liveSkeletonVisibleCheckButton, sf::Rect<sf::Uint32>(0, 4, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));

	recordingLabel->SetText("Skeleton Recording:");
	recordingLabel->SetAlignment(sf::Vector2f(0.f, 0.75f));
	table->Attach(recordingLabel,      sf::Rect<sf::Uint32>(0,  5, colspan    , 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	table->Attach(animLayersComboBox,  sf::Rect<sf::Uint32>(0,  6, colspan    , 1), sfg::Table::FILL, sfg::Table::FILL);
	table->SetRowSpacing(6, 5.f);
	table->Attach(recordStartButton,   sf::Rect<sf::Uint32>(0,  7, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(recordStopButton,    sf::Rect<sf::Uint32>(3,  7, colspan / 2, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->SetRowSpacing(7, 2.5f);
	table->Attach(recordClearButton,   sf::Rect<sf::Uint32>(0,  8, colspan,     1), sfg::Table::FILL, sfg::Table::FILL);
	table->SetRowSpacing(8, 2.5f);
	table->Attach(startLayeringButton, sf::Rect<sf::Uint32>(0,  9, colspan    , 1), sfg::Table::FILL, sfg::Table::FILL);

	playbackLabel->SetAlignment(sf::Vector2f(0.f, 0.75f));
	table->Attach(playbackLabel,       sf::Rect<sf::Uint32>(0, 10, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.));
	table->Attach(playbackProgressBar, sf::Rect<sf::Uint32>(0, 11, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 10.f));
	table->SetRowSpacing(11, 5.f);
	table->Attach(playbackFirstButton,    sf::Rect<sf::Uint32>(0, 12, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackPreviousButton, sf::Rect<sf::Uint32>(1, 12, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackStopButton,     sf::Rect<sf::Uint32>(2, 12, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackStartButton,    sf::Rect<sf::Uint32>(3, 12, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackNextButton,     sf::Rect<sf::Uint32>(4, 12, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->Attach(playbackLastButton,     sf::Rect<sf::Uint32>(5, 12, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
	table->SetRowSpacing(12, 5.f);
	sfg::Label::Ptr deltaScaleLabel(sfg::Label::Create("Delta"));
	table->Attach(deltaScaleLabel,    sf::Rect<sf::Uint32>(0, 13,           2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 2.f));
	table->Attach(playbackDeltaScale, sf::Rect<sf::Uint32>(2, 13, colspan - 2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 2.f));

	table->SetRowSpacing(13, 5.f);
	table->Attach(renderPathCheckButton, sf::Rect<sf::Uint32>(0, 14, colspan, 1), sfg::Table::FILL, sfg::Table::FILL);

	infoLabel->SetAlignment(sf::Vector2f(0.f, 0.5f));
	table->Attach(infoLabel, sf::Rect<sf::Uint32>(0, 14, colspan, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 10.f));

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
	renderPathCheckButton         ->GetSignal(sfg::CheckButton::OnLeftClick).Connect(&GUI::onRenderPathCheckButtonClick,          this);

	playbackProgressBar   ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackProgressBarClick,    this);
	playbackFirstButton   ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackFirstButtonClick,    this);
	playbackPreviousButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackPreviousButtonClick, this);
	playbackStopButton    ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackStopButtonClick,     this);
	playbackStartButton   ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackStartButtonClick,    this);
	playbackNextButton    ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackNextButtonClick,     this);
	playbackLastButton    ->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onPlaybackLastButtonClick,     this);
	playbackDeltaScale    ->GetSignal(sfg::Scale::OnLeftClick ).Connect(&GUI::onPlaybackDeltaScaleClick,     this);

	startLayeringButton->GetSignal(sfg::Button::OnLeftClick ).Connect(&GUI::onStartLayeringButtonClick, this);
	animLayersComboBox ->GetSignal(sfg::ComboBox::OnSelect  ).Connect(&GUI::onAnimLayersComboBoxSelect, this);
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

void GUI::setProgressFraction( const float fraction )
{
	// Clamp [0..1]
	float frac = fraction;
	     if (fraction < 0.f) frac = 0.f;
	else if (fraction > 1.f) frac = 1.f;

	playbackProgressBar->SetFraction(frac);
}

void GUI::appendLayerItem( const std::string& text )
{
	animLayersComboBox->AppendItem(text);
	size_t index = animLayersComboBox->GetItemCount();
	animLayersComboBox->SelectItem(index - 1);
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

void GUI::onPlaybackProgressBarClick()
{
	// TODO : msg::PlaybackSetProgress???Message
	// send message back to glwindow to change playbackTime
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

void GUI::onStartLayeringButtonClick()
{
	msg::gDispatcher.dispatchMessage(msg::StartLayeringMessage());
}

void GUI::onAnimLayersComboBoxSelect()
{
	const std::string layerName = animLayersComboBox->GetSelectedText();
	msg::gDispatcher.dispatchMessage(msg::LayerSelectMessage(layerName));
}

void GUI::onRenderPathCheckButtonClick()
{
	const bool active = renderPathCheckButton->IsActive();
	if (active) {
		msg::gDispatcher.dispatchMessage(msg::ShowBonePathMessage());
	} else {
		msg::gDispatcher.dispatchMessage(msg::HideBonePathMessage());
	}
}
