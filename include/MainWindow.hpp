#pragma once

#include "Context.hpp"
#include "HotKeyComponent.hpp"
#include "Primitives.hpp"
#include "SWManager.hpp"

#include <memory>
#include <stdexcept>
#include <vector>

/**
 * @brief The MainWindow class
 *
 * Application -> MainWindow -> SWManager -> All the SubWindows
 *
 */
class MainWindow
{
public:
	MainWindow();

	void EventLoopIterationStart();
	void HandleUserEvent(Event event);
	void PreUpdate();
	void Update();
	void Draw() const;

	Vector2i GetWindowSize() const;

private:
	void RegisterSubWindows();
	void HandleResizeEvent();
	void InitializeHotKeyActions();

private:
	Recti			mRect;
	SWManager		mSWManager;
	HotKeyComponent mHotKeyComponent;
};
