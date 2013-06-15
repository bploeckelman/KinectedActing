#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>

#include <iostream>

bool quit = false;

void onButtonClick() {
	std::cout << "click!\n";
	quit = true;
}


int main()
{
	// Render windows
	sf::VideoMode desktopVideoMode(sf::VideoMode::getDesktopMode());
	sf::VideoMode glVideoMode(desktopVideoMode.width - 256 - 100, desktopVideoMode.height - 100, 32);
	sf::VideoMode guiVideoMode(256, desktopVideoMode.height - 100, 32);
	sf::RenderWindow glWindow(glVideoMode, "OpenGL Window");
	sf::RenderWindow guiWindow(guiVideoMode, "GUI Window");

	glWindow.setFramerateLimit(60);
	guiWindow.setFramerateLimit(30);
	glWindow.setPosition(sf::Vector2i(10 + guiVideoMode.width + 20, 10));
	guiWindow.setPosition(sf::Vector2i(10, 10));

	// GUI
	sfg::SFGUI sfgui;
	sfg::Desktop desktop;
	sfg::Box::Ptr box(sfg::Box::Create(sfg::Box::VERTICAL));
	sfg::Fixed::Ptr fixed(sfg::Fixed::Create());
	sfg::Window::Ptr window(sfg::Window::Create());
	sfg::Button::Ptr button(sfg::Button::Create("Quit"));

	button->GetSignal(sfg::Button::OnLeftClick).Connect(onButtonClick);
	fixed->Put(button, sf::Vector2f(0, 0));
	box->Pack(fixed);
	window->SetRequisition(sf::Vector2f(guiWindow.getSize()));
	window->SetTitle("Kinected Acting");
	window->SetStyle(sfg::Window::Style::NO_STYLE);
	window->Add(box);
	desktop.Add(window);

	// Main Loop
	while (glWindow.isOpen() && guiWindow.isOpen()) {
		if (quit) {
			glWindow.close();
			guiWindow.close();
			break;
		}

		sf::Event event;

		// Update gui window
		while (guiWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				glWindow.close();
				guiWindow.close();
			}
			desktop.HandleEvent(event);
		}

		// Draw gui window
		guiWindow.setActive();
		guiWindow.clear(sf::Color::Black);

		// Update and draw gui
		desktop.Update(1.f);
		guiWindow.pushGLStates();
		sfgui.Display(guiWindow);
		guiWindow.popGLStates();

		guiWindow.display();

		// Update gl window
		while (glWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				glWindow.close();
				guiWindow.close();
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				std::cout << "gl click\n";
			}
		}

		// Draw gl window
		glWindow.setActive();
		glWindow.clear(sf::Color::Black);
		glWindow.display();
	}

	return 0;
}
