#include "Kinect/KinectDevice.h"
#include "UserInterface.h"

#include <string>


GUI::GUI(sf::RenderWindow& renderWindow)
	: renderWindow(renderWindow)
	, sfgui()
	, desktop()
	, window(sfg::Window::Create())
	, box(sfg::Box::Create(sfg::Box::Orientation::VERTICAL))
	, quitButton(sfg::Button::Create("Quit"))
	, kinectTable(sfg::Table::Create())
	, kinectScrolledWindow(sfg::ScrolledWindow::Create())
	, kinectScrolledWindowBox(sfg::Box::Create(sfg::Box::HORIZONTAL))
	, kinectIdLabel(sfg::Label::Create())
{}

GUI::~GUI()
{}

void GUI::update()
{
	desktop.Update(1.f);
}

void GUI::render()
{
	renderWindow.pushGLStates();
	sfgui.Display(renderWindow);
	renderWindow.popGLStates();
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
	const sf::Vector2f winsize(renderWindow.getSize());

	box->SetRequisition(sf::Vector2f(winsize.x - 20.f, winsize.y - 50.f));
	box->Pack(quitButton);

	kinectIdLabel->SetText("[???]");
	kinectScrolledWindowBox->SetRequisition(sf::Vector2f(winsize.x - 100.f, 10.f));
	kinectScrolledWindowBox->Pack(kinectIdLabel, false, false);

	kinectScrolledWindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_ALWAYS | sfg::ScrolledWindow::VERTICAL_NEVER);
	kinectScrolledWindow->SetHorizontalAdjustment(sfg::Adjustment::Create(0.f, 0.f, 1.f, 0.1f, 0.5f, 0.5f));
	kinectScrolledWindow->AddWithViewport(kinectScrolledWindowBox);
	kinectScrolledWindow->SetRequisition(sf::Vector2f(winsize.x - 100.f, 0.f));

	kinectTable->SetRequisition(sf::Vector2f(winsize.x - 20.f, winsize.y - 50.f));
	// Attach widget:  (widgetPtr, <col idx, row idx, col span, row span>, horizontal packing, vertical packing, padding)
	kinectTable->Attach(kinectScrolledWindow, sf::Rect<sf::Uint32>(0, 0, 1, 1), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(5.f, 5.f));

	box->Pack(kinectTable);

	window->SetTitle("Kinected Acting");
	window->Add(box);

	desktop.Add(window);
}

void GUI::connectSignals()
{
	quitButton->GetSignal(sfg::Button::OnLeftClick).Connect(&GUI::onQuitButtonClick, this);
}

void GUI::onQuitButtonClick()
{
	renderWindow.close();
}

void GUI::setKinectIdLabel(const std::string& text)
{
	kinectIdLabel->SetText(text);
}
