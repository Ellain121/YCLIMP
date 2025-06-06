#include "PlayerSW.hpp"

#include "FileManager.hpp"
#include "SWManager.hpp"
#include "SubWindow.hpp"

#include <memory>

#define DIR_TEST \
	"/home/ellain/Music/Fall Out Boy AAC 320/2018-01-19 - M A N I A [AU - 5766373]/"
#include "MusicPlayer.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <thread>

PlayerSW::PlayerSW(const Recti& bounds, Context context, SWHolderType inHolderType)
	: SubWindow{ESWTypes::Player, bounds, context, inHolderType}
	, mFileSelector{
		  Recti{bounds.left + 1, bounds.top + 1, bounds.width - 2, bounds.height - 2}}
{
	InitializeHotKeysActions();
}

void PlayerSW::InitializeHotKeysActions()
{
	mHotKeyComponent.InsertBindingAction(std::make_pair("up",
		[this]()
		{
			//
			this->mFileSelector.SelectUp();
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("down",
		[this]()
		{
			//
			this->mFileSelector.SelectDown();
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("play_selected",
		[this]()
		{
			auto		file = this->mFileSelector.GetSelected().value();
			std::string bytes = file->GetSongData();
			this->GetContext().swManager.AddSWEvent(SWEvent{ESWTypes::PlayerStatus,
				ESWEventAction::PlayMusicBytes, std::move(bytes), file->name});
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("shuffle_tracks", [this]() {	//
		this->mFileSelector.ShuffleElements();
	}));
}

void PlayerSW::HandleUserEvent(Event event, bool rawInputMode)
{
	SubWindow::HandleUserEvent(event, rawInputMode);
}

void PlayerSW::HandleSWEvents(std::vector<SWEvent>& swEvents)
{
	for (int i = swEvents.size() - 1; i >= 0; --i)
	{
		auto& swEvent = swEvents[i];
		if (swEvent.target != GetSWType())
			continue;

		if (swEvent.action == ESWEventAction::PlayedMusicDone)
		{
			std::string repeatMode = std::get<std::string>(swEvent.data);
			if (repeatMode == MusicPlayer::GetStringName(ERepeatMode::None))
			{
				mFileSelector.SelectDown();
			}
			else if (repeatMode == MusicPlayer::GetStringName(ERepeatMode::All))
			{
				mFileSelector.SelectDown();
				auto size = mFileSelector.GetElementsSize();
				auto indx = mFileSelector.GetSelectedPosition();
				if (indx + 1 == size)
				{
					mFileSelector.SelectByIndx(0);
				}
			}
			else if (repeatMode == MusicPlayer::GetStringName(ERepeatMode::One))
			{
				// ???
			}
			auto file = mFileSelector.GetSelected().value();
			// {
			std::string bytes = file->GetSongData();
			this->GetContext().swManager.AddSWEvent(SWEvent{ESWTypes::PlayerStatus,
				ESWEventAction::PlayMusicBytes, std::move(bytes), file->name});

			swEvents.erase(swEvents.begin() + i);
		}

		// open new files and overwrite/append_to old ones
		else if (swEvent.action == ESWEventAction::OpenNewDirInPlayerSW ||
				 swEvent.action == ESWEventAction::AppendNewDirInPlayerSW)
		{
			File_Ptr newDir = std::get<File_Ptr>(swEvent.data);
			auto	 vec = newDir->GetDirFiles(true, true, true);

			// pass true(overwrite) if action is openNewDir...
			mFileSelector.Load(std::move(vec),
				static_cast<bool>(
					swEvent.action == ESWEventAction::OpenNewDirInPlayerSW));

			swEvents.erase(swEvents.begin() + i);
		}
		// open new file  and overwrite/append_to old ones
		else if (swEvent.action == ESWEventAction::OpenOneFileInPlayerSW ||
				 swEvent.action == ESWEventAction::AppendOneFileToPlayerSW)
		{
			File_Ptr file = std::get<File_Ptr>(swEvent.data);

			assert(file != nullptr);
			{
				mFileSelector.Load(
					file, static_cast<bool>(
							  swEvent.action == ESWEventAction::OpenOneFileInPlayerSW));
			}
			swEvents.erase(swEvents.begin() + i);
		}
	}
}

void PlayerSW::Update()
{
	mFileSelector.Update();
}

void PlayerSW::Draw() const
{
	if (!IsVisible())
		return;

	SubWindow::DrawBoundsRectangle();

	mFileSelector.Draw();

	// DrawElemSelectorInfo();
}

void PlayerSW::DrawElemSelectorInfo() const
{
	std::size_t total = mFileSelector.GetElementsSize();
	std::size_t pos = std::min(mFileSelector.GetSelectedPosition() + 1, total);
	std::string info = std::to_string(pos) + "/" + std::to_string(total);

	const auto& bounds = GetBounds();
	int			y = bounds.top + bounds.height;
	int			x = bounds.left + bounds.width - info.size();

	mvprintw(y, x, info.c_str());
}

void PlayerSW::HandleResizeEvent(const Recti& newRect)
{
	SubWindow::HandleResizeEvent(newRect);
	mFileSelector.HandleResize(
		Recti{newRect.left + 1, newRect.top + 1, newRect.width - 2, newRect.height - 2});
}
