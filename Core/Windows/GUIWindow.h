#pragma once
#include "Window.h"
#include "Core/GUI/UserInterface.h"
#include "Core/Messages/Messages.h"

#include <string>


class GUIWindow : public Window, msg::Handler
{
public:
	GUIWindow(const std::string& title, App& app);
	virtual ~GUIWindow();

	void init();
	void update();
	void render();

	GUI& getGUI();
	const GUI& getGUI() const;

private:
	GUI gui;

	// Message processing methods ----------------------------
	void registerMessageHandlers();

public:
	void process(const msg::SetRecordingLabelMessage   *message);
	void process(const msg::PlaybackSetProgressMessage *message);
	void process(const msg::SetInfoLabelMessage        *message);
	void process(const msg::AddLayerItemMessage        *message);

};


inline GUI& GUIWindow::getGUI() { return gui; }
inline const GUI& GUIWindow::getGUI() const { return gui; }
