#include "UserInterface.h"

#include <string>


GUI::GUI(sf::RenderWindow& renderWindow)
	: renderWindow(renderWindow)
	, sfgui()
	, desktop()
	, window(sfg::Window::Create())
	, fixed(sfg::Fixed::Create())
	, quitButton(sfg::Button::Create("Quit"))
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
	fixed->SetRequisition(sf::Vector2f(renderWindow.getSize()));
	fixed->Put(quitButton, sf::Vector2f(0, 0));
	window->SetTitle("Kinected Acting");
	window->Add(fixed);
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
