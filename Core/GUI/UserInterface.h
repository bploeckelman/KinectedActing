#pragma once
#include <SFGUI/SFGUI.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Core/Windows/Window.h"


class GUI
{
public:
	GUI(Window& parentWindow);
	~GUI();

	void initialize();
	void update();
	void render();
	void handleEvent(const sf::Event& event);

public: // External interface
	void setKinectIdLabel(const std::string& text);
	void setInfoLabel(const std::string& text);

	bool isRecording() const;
	void startRecording();
	void stopRecording();

private:
	void layoutWidgets();
	void connectSignals();

	// Signal methods -----------------
	void onQuitButtonClick();
	void onStartKinectButtonClick();
	void onStopKinectButtonClick();
	void onRecordStartButtonClick();
	void onRecordStopButtonClick();

private:
	Window& parentWindow;

	sfg::SFGUI sfgui;
	sfg::Desktop desktop;
	sfg::Window::Ptr window;

	sfg::Box::Ptr box;
	sfg::Button::Ptr quitButton;

	sfg::Table::Ptr kinectTable;
	sfg::ScrolledWindow::Ptr kinectScrolledWindow;
	sfg::Box::Ptr kinectScrolledWindowBox;
	sfg::Label::Ptr kinectLabel;
	sfg::Label::Ptr kinectIdLabel;

	sfg::Button::Ptr startKinectButton;
	sfg::Button::Ptr stopKinectButton;

	sfg::Button::Ptr recordStartButton;
	sfg::Button::Ptr recordStopButton;

	sfg::Label::Ptr infoLabel;

	bool recording;

};


inline bool GUI::isRecording() const { return recording; }
inline void GUI::startRecording() { recording = true;  }
inline void GUI::stopRecording()  { recording = false; }
