#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include <string>


class Window 
{
public:
	Window(const std::string& title) : title(title), videoMode(), window() {};
	virtual ~Window() {};

	virtual void update() = 0;
	virtual void render() = 0;

	sf::VideoMode& getVideoMode();
	const sf::VideoMode& getVideoMode() const;

	sf::RenderWindow& getWindow();
	const sf::RenderWindow& getWindow() const;

protected:
	std::string title;
	sf::VideoMode videoMode;
	sf::RenderWindow window;

};


inline sf::VideoMode& Window::getVideoMode() { return videoMode; }
inline const sf::VideoMode& Window::getVideoMode() const { return videoMode; }

inline sf::RenderWindow& Window::getWindow() { return window; }
inline const sf::RenderWindow& Window::getWindow() const { return window; }
