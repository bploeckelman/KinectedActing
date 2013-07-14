#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include <string>

class App;


class Window 
{
public:
	Window(const std::string& title, App& app) : title(title), app(app), videoMode(), window() {};
	virtual ~Window() {};

	virtual void init() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	App& getApp();
	const App& getApp() const;

	sf::VideoMode& getVideoMode();
	const sf::VideoMode& getVideoMode() const;

	sf::RenderWindow& getWindow();
	const sf::RenderWindow& getWindow() const;

protected:
	virtual void registerMessageHandlers() = 0;

	App& app;
	std::string title;
	sf::VideoMode videoMode;
	sf::RenderWindow window;

};


inline App& Window::getApp() { return app; }
inline const App& Window::getApp() const { return app; }

inline sf::VideoMode& Window::getVideoMode() { return videoMode; }
inline const sf::VideoMode& Window::getVideoMode() const { return videoMode; }

inline sf::RenderWindow& Window::getWindow() { return window; }
inline const sf::RenderWindow& Window::getWindow() const { return window; }
