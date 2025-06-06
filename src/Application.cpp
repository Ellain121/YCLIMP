#include "Application.hpp"

#include "Debug.hpp"
#include "Primitives.hpp"
#include "ncurses.h"
#undef OK	 // ncurses for some reason defines OK in global scope
// which is not good for other libraries

#include <string>

Application::Application()
	: mbProgramActive{true}
{
	Debug::Init();
}

void Application::Run()
{
	while (mbProgramActive)
	{
		EventLoopIterationStart();
		ProcessEvents();
		PreUpdate();
		Update();
		Draw();
	}
}

void Application::EventLoopIterationStart()
{
	mWindow.EventLoopIterationStart();
}

void Application::ProcessEvents()
{
	ProcessUserEvents();
}

void Application::PreUpdate()
{
	mWindow.PreUpdate();
}

void Application::Update()
{
	mWindow.Update();
}

void Application::Draw() const
{
	clear();

	mWindow.Draw();

	refresh();
}

void Application::ProcessUserEvents()
{
	int	  k = getch();
	Event event{k};
	if (event.key == EEventKey::Q)
	{
		mbProgramActive = false;
	}
	mWindow.HandleUserEvent(event);
}
