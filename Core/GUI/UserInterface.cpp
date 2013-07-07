#include <GL/glew.h>

#include "Kinect/KinectDevice.h"

#include "UserInterface.h"
#include "Core/App.h"
#include "Core/Windows/Window.h"

#include <string>


GUI::GUI(Window& parentWindow)
	: parentWindow(parentWindow)
	, sfgui()
	, desktop()
	, window(sfg::Window::Create())
	, box(sfg::Box::Create(sfg::Box::Orientation::VERTICAL))
	, quitButton(sfg::Button::Create("Quit"))
	, kinectTable(sfg::Table::Create())
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
	, infoLabel(sfg::Label::Create(""))
	, liveSkeletonVisibleCheckButton(sfg::CheckButton::Create("Show Live Skeleton"))
	, recording(false)
	, liveSkeletonVisible(true)
	, clearKeyFrames(false)
{}

GUI::~GUI()
{}

void GUI::update()
{
	desktop.Update(1.f);
}

void GUI::render()
{
	parentWindow.getWindow().pushGLStates();
	sfgui.Display(parentWindow.getWindow());
	parentWindow.getWindow().popGLStates();
}

void GUI::initialize()
{
	layoutWidgets();
	connectSignals();
}

void GUI::handleEvent(const sf::Event& event)
{
	desktop.HandleEvent(event);
}

void GUI::layoutWidgets()
{
	const sf::Vector2f winsize(parentWindow.getWindow().getSize());

	kinectLabel->SetText("Kinect Sensor:");
	kinectLabel->SetAlignment(sf::Vector2f(0.f,0.75f));

	kinectIdLabel->SetText("[ offline ]");
	kinectScrolledWindowBox->SetRequisition(sf::Vector2f(winsize.x - 30.f, 0.f));
	kinectScrolledWindowBox->Pack(kinectIdLabel, false, false);

	kinectScrolledWindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_ALWAYS | sfg::ScrolledWindow::VERTICAL_NEVER);
	kinectScrolledWindow->SetHorizontalAdjustment(sfg::Adjustment::Create(0.f, 0.f, 1.f, 0.1f, 0.5f, 0.5f));
	kinectScrolledWindow->AddWithViewport(kinectScrolledWindowBox);
	kinectScrolledWindow->SetRequisition(sf::Vector2f(winsize.x - 30.f, 0.f));

	quitButton->SetRequisition(sf::Vector2f(winsize.x - 25.f, 20.f));

	startKinectButton->SetRequisition(sf::Vector2f((winsize.x - 30.f) / 2.f, 20.f));
	stopKinectButton->SetRequisition(sf::Vector2f((winsize.x - 30.f) / 2.f, 20.f));

	recordStartButton->SetRequisition(sf::Vector2f((winsize.x - 30.f) / 2.f, 20.f));
	recordStopButton->SetRequisition(sf::Vector2f((winsize.x - 30.f) / 2.f, 20.f));
	recordClearButton->SetRequisition(sf::Vector2f(winsize.x - 25.f, 20.f));

	liveSkeletonVisibleCheckButton->SetActive(true);

	kinectTable->SetRequisition(sf::Vector2f(winsize.x - 30.f, winsize.y - 50.f));
	// Attach widget:  (widgetPtr, <col idx, row idx, col span, row span>, horizontal packing, vertical packing, padding)
	kinectTable->Attach(quitButton,           sf::Rect<sf::Uint32>(0, 0, 2, 1), 0, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

	kinectTable->Attach(kinectLabel,          sf::Rect<sf::Uint32>(0, 1, 2, 1), sfg::Table::FILL,   sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	kinectTable->Attach(kinectScrolledWindow, sf::Rect<sf::Uint32>(0, 2, 2, 1), sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));

	sfg::Label::Ptr deviceLabel = sfg::Label::Create("Kinect Device:");
	deviceLabel->SetAlignment(sf::Vector2f(0.f, 0.75f));
	kinectTable->Attach(deviceLabel,          sf::Rect<sf::Uint32>(0, 3, 2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	kinectTable->Attach(startKinectButton,    sf::Rect<sf::Uint32>(0, 4, 1, 1), 0, 0, sf::Vector2f(2.5f, 0.f));
	kinectTable->Attach(stopKinectButton,     sf::Rect<sf::Uint32>(1, 4, 1, 1), 0, 0, sf::Vector2f(2.5f, 0.f));

	kinectTable->Attach(liveSkeletonVisibleCheckButton, sf::Rect<sf::Uint32>(0, 5, 2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));

	recordingLabel->SetText("Skeleton Recording:");
	recordingLabel->SetAlignment(sf::Vector2f(0.f, 0.75f));
	kinectTable->Attach(recordingLabel,       sf::Rect<sf::Uint32>(0, 6, 2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 8.f));
	kinectTable->Attach(recordStartButton,    sf::Rect<sf::Uint32>(0, 7, 1, 1), 0, 0, sf::Vector2f(2.5f, 0.f));
	kinectTable->Attach(recordStopButton,     sf::Rect<sf::Uint32>(1, 7, 1, 1), 0, 0, sf::Vector2f(2.5f, 0.f));
	kinectTable->Attach(recordClearButton,    sf::Rect<sf::Uint32>(0, 8, 2, 1), 0, 0/*sfg::Table::FILL*/, sf::Vector2f(0.f, 0.f));

	infoLabel->SetAlignment(sf::Vector2f(0.f, 0.5f));
	kinectTable->Attach(infoLabel,            sf::Rect<sf::Uint32>(0, 9, 2, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.f, 10.f));

	box->Pack(kinectTable);

	window->SetTitle("Kinected Acting");
	window->Add(box);

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
}

void GUI::onQuitButtonClick()
{
	parentWindow.getWindow().close();
}

void GUI::onStartKinectButtonClick()
{
	parentWindow.getApp().getKinect().init();
	kinectIdLabel->SetText(parentWindow.getApp().getKinect().getDeviceId());
}

void GUI::onStopKinectButtonClick()
{
	parentWindow.getApp().getKinect().shutdown();
	kinectIdLabel->SetText("[ offline ]");
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
	startRecording();
}

void GUI::onRecordStopButtonClick()
{
	stopRecording();
}

void GUI::onRecordClearButtonClick()
{
	clearKeyFrames = true;
}

void GUI::onLiveSkeletonVisibleCheckButtonClick()
{
	liveSkeletonVisible = liveSkeletonVisibleCheckButton->IsActive();
}
