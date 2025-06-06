#include "MainWindow.hpp"

#include "AppFilesSW.hpp"
#include "DialogSW.hpp"
#include "FileManager.hpp"
#include "LocalFilesSW.hpp"
#include "PlayerSW.hpp"
#include "PlayerStatusSW.hpp"
#include "Primitives.hpp"
#include "YandexFilesSW.hpp"
#include "ncurses.h"
#undef OK

MainWindow::MainWindow()
	: mRect{0, 0, COLS, LINES}
	, mSWManager{Recti{0, 0, mRect.width, mRect.height}, *this}
	, mHotKeyComponent{"MainWindow"}
{
	InitializeHotKeyActions();
	RegisterSubWindows();

	mSWManager.InitStartupState();
}

void MainWindow::EventLoopIterationStart()
{
	mSWManager.EventLoopIterationStart();
}

void MainWindow::InitializeHotKeyActions()
{
}

void MainWindow::RegisterSubWindows()
{
	mSWManager.RegisterSW<PlayerStatusSW>(ESWTypes::PlayerStatus);
	mSWManager.RegisterSW<PlayerSW>(ESWTypes::Player);
	mSWManager.RegisterSW<LocalFilesSW>(ESWTypes::LocalFiles);
	mSWManager.RegisterSW<YandexFilesSW>(ESWTypes::YandexFiles);
	mSWManager.RegisterSW<AppFilesSW>(ESWTypes::AppFiles);
	mSWManager.RegisterSW<DialogSW>(ESWTypes::Dialog);
}

void MainWindow::HandleUserEvent(Event event)
{
	// MainWindow Events:
	if (event.key == EEventKey::KeyResize)
	{
		HandleResizeEvent();
	}

	mHotKeyComponent.HandleUserEvent(event, false);
	mSWManager.HandleUserEvent(event);
}

void MainWindow::PreUpdate()
{
	mSWManager.HandleSWEvents();
}

Vector2i MainWindow::GetWindowSize() const
{
	return Vector2i{COLS, LINES};
}

void MainWindow::HandleResizeEvent()
{
	auto wSize = GetWindowSize();
	mRect = Rect{0, 0, wSize.x, wSize.y};
	mSWManager.HandleResizeEvent(mRect);
}

void MainWindow::Update()
{
	mSWManager.Update();
}

void MainWindow::Draw() const
{
	mSWManager.Draw();
}
