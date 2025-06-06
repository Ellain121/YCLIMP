#include "PlayerStatusSW.hpp"

#include "SubWindow.hpp"
#include "Utility.hpp"
#include "ncurses.h"
#undef OK

#include <algorithm>
#include <format>

PlayerStatusSW::PlayerStatusSW(
	const Recti& bounds, Context context, SWHolderType inHolderType)
	: SubWindow{ESWTypes::PlayerStatus, bounds, context, inHolderType}
	, mMusicPlayer{&GetContext().swManager}
	, mProgressBar{'#', 50}
{
	InitializeHotKeysActions();
}

void PlayerStatusSW::InitializeHotKeysActions()
{
	mHotKeyComponent.InsertBindingAction(std::make_pair(
		"pause_play", [this]() { this->mMusicPlayer.ToggleContinuePause(); }));

	mHotKeyComponent.InsertBindingAction(std::make_pair(
		"fast_forward", [this]() { this->mMusicPlayer.FastForward5Sec(); }));

	mHotKeyComponent.InsertBindingAction(
		std::make_pair("rewind", [this]() { this->mMusicPlayer.Rewind5Sec(); }));

	mHotKeyComponent.InsertBindingAction(
		std::make_pair("volume_up", [this]() { this->mMusicPlayer.VolumeUp5Percent(); }));

	mHotKeyComponent.InsertBindingAction(std::make_pair(
		"volume_down", [this]() { this->mMusicPlayer.VolumeDown5Percent(); }));

	mHotKeyComponent.InsertBindingAction(std::make_pair(
		"toggle_repeat_mode", [this]() { this->mMusicPlayer.ToggleRepeatMode(); }));
}

void PlayerStatusSW::HandleUserEvent(Event event, bool rawInputMode)
{
	SubWindow::HandleUserEvent(event, rawInputMode);
}

void PlayerStatusSW::HandleSWEvents(std::vector<SWEvent>& swEvents)
{
	for (int i = swEvents.size() - 1; i >= 0; --i)
	{
		auto& swEvent = swEvents[i];
		if (swEvent.target != GetSWType())
			continue;

		if (swEvent.action == ESWEventAction::PlayMusicFile)
		{
			mMusicPlayer.PlayMusicFromFile(std::get<std::string>(swEvent.data));
			swEvents.erase(swEvents.begin() + i);
		}
		else if (swEvent.action == ESWEventAction::PlayMusicBytes)
		{
			mMusicPlayer.PlayMusicFromBytes(
				std::get<std::string>(swEvent.data), swEvent.data_2);
			swEvents.erase(swEvents.begin() + i);
		}
	}
}

void PlayerStatusSW::Update()
{
	mMusicPlayer.Update();
}

void PlayerStatusSW::Draw() const
{
	if (IsVisible() == false)
		return;

	SubWindow::DrawBoundsRectangle();

	const auto& bounds = GetBounds();
	const auto& musicName = mMusicPlayer.GetCurMusicName();

	if (!musicName.empty())
	{
		// Draw current song time in such format: (1:15/3:31)
		Time progress{static_cast<int>(mMusicPlayer.GetCurSongProgress())};
		Time duration{static_cast<int>(mMusicPlayer.GetCurSongDuration())};
		int	 x = bounds.left + 5;
		int	 y = bounds.top + 1;
		PrintwInLimitedX(Vector2i{x, y}, musicName, bounds.width - x);

		// Draw time progress
		std::string timeInfo = std::format("({:02}:{:02}/{}:{:02})", progress.min,
			progress.sec, duration.min, duration.sec);
		int			xOffset = musicName.size() + 2;
		PrintwInLimitedX(Vector2i{x + xOffset, y}, timeInfo, bounds.width, 0, false);

		// Draw Progress Bar
		std::string prgBar = mProgressBar.GetProgressBar(
			static_cast<int>(mMusicPlayer.GetCurSongProgressPercent()),
			bounds.width - x - 2);
		PrintwInLimitedX(Vector2i{x, y + 1}, prgBar, bounds.width - 1);
		// mvprintw(y + 1, x, prgBar.c_str());

		// Draw current volume level
		std::string volumeInfo =
			std::string{"Volume: "} +
			std::to_string(int(mMusicPlayer.GetVolume() * 100 + 0.5f)) + "%%";
		mvprintw(y + 2, x, volumeInfo.c_str());

		// Draw current repeat mode
		std::string repeatModeInfo =
			std::string{"Repeat: "} + mMusicPlayer.GetRepeatMode();
		mvprintw(y + 2, x + volumeInfo.size() + 2, repeatModeInfo.c_str());
	}
}

void PlayerStatusSW::HandleResizeEvent(const Recti& newRect)
{
	SubWindow::HandleResizeEvent(newRect);
}
