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
	, kinectLabel(sfg::Label::Create("Kinect"))
	, kinectIdLabel(sfg::Label::Create())
	, startKinectButton(sfg::Button::Create("Start Kinect"))
	, stopKinectButton(sfg::Button::Create("Stop Kinect"))
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

	box->SetRequisition(sf::Vector2f(winsize.x - 30.f, winsize.y - 50.f));
	box->Pack(quitButton);

	kinectLabel->SetText("Kinect          ");
	kinectIdLabel->SetText("[ Kinect offline ]");
	kinectScrolledWindowBox->SetRequisition(sf::Vector2f(winsize.x - 100.f, 0.f));
	kinectScrolledWindowBox->Pack(kinectIdLabel, false, false);

	kinectScrolledWindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_ALWAYS | sfg::ScrolledWindow::VERTICAL_NEVER);
	kinectScrolledWindow->SetHorizontalAdjustment(sfg::Adjustment::Create(0.f, 0.f, 1.f, 0.1f, 0.5f, 0.5f));
	kinectScrolledWindow->AddWithViewport(kinectScrolledWindowBox);
	kinectScrolledWindow->SetRequisition(sf::Vector2f(winsize.x - 100.f, 0.f));

	startKinectButton->SetRequisition(sf::Vector2f(winsize.x - 30.f, 20.f));
	stopKinectButton->SetRequisition(sf::Vector2f(winsize.x - 30.f, 20.f));

	kinectTable->SetRequisition(sf::Vector2f(winsize.x - 30.f, winsize.y - 50.f));
	kinectTable->SetRowSpacings(1.f);
	// Attach widget:  (widgetPtr, <col idx, row idx, col span, row span>, horizontal packing, vertical packing, padding)
	kinectTable->Attach(kinectLabel, sf::Rect<sf::Uint32>(0, 0, 1, 1), sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(5.f, 5.f));
	kinectTable->Attach(kinectScrolledWindow, sf::Rect<sf::Uint32>(0, 1, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.f, 0.f));
	kinectTable->Attach(startKinectButton, sf::Rect<sf::Uint32>(0, 2, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(5.f, 5.f));
	kinectTable->Attach(stopKinectButton, sf::Rect<sf::Uint32>(0, 3, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(5.f, 5.f));

	box->Pack(kinectTable);

	window->SetTitle("Kinected Acting");
	window->Add(box);

	desktop.Add(window);
}

void GUI::connectSignals()
{
	quitButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onQuitButtonClick, this);
	startKinectButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onStartKinectButtonClick, this);
	stopKinectButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onStopKinectButtonClick, this);
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
	kinectIdLabel->SetText("[ Kinect offline ]");
}

void GUI::setKinectIdLabel(const std::string& text)
{
	kinectIdLabel->SetText(text);
}
