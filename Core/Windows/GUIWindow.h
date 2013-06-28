#pragma once
#include "Window.h"
#include "Core/GUI/UserInterface.h"

#include <string>


class GUIWindow : public Window
{
public:
	GUIWindow(const std::string& title, App& app);
	virtual ~GUIWindow();

	void init();
	void update();
	void render();

	const GUI& getGUI() const;

private:
	GUI gui;
	
};


inline const GUI& GUIWindow::getGUI() const { return gui; }
