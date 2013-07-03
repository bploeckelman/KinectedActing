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

	GUI& getGUI();
	const GUI& getGUI() const;

private:
	GUI gui;
	
};


inline GUI& GUIWindow::getGUI() { return gui; }
inline const GUI& GUIWindow::getGUI() const { return gui; }
