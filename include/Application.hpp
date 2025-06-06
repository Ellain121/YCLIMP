#pragma once

#include "MainWindow.hpp"

/**
 * @brief Main class of the app
 *
 * Programm starts with the moethod Run. Inside run there is a event-based loop@n
 * Loop scheme(not full):@n
 * 1) Getting user input@n
 * 2) Process user input@n
 * 3) Update(Almost not used and all the changes inside ProcessEvents/HandleUserInpnut)@n
 * 3) Draw current state@n
 */

class Application
{
public:
	Application();
	void Run();

private:
	void EventLoopIterationStart();
	void ProcessEvents();
	void Draw() const;
	void PreUpdate();
	void Update();

	void ProcessUserEvents();

private:
	MainWindow mWindow;
	bool	   mbProgramActive;
};
