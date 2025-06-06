#pragma once

#include "MusicPlayer.hpp"
#include "SubWindow.hpp"
#include "Utility.hpp"

#include <vector>

/**
 * @brief The PlayerStatusSW class;
 * Player Status SubWindow responsible for current track information, like track progress, track name, etc.
 */
class PlayerStatusSW : public SubWindow
{
public:
	PlayerStatusSW(const Recti& bounds, Context context,
		SWHolderType inHolderType = SWHolderType::None);

	virtual void HandleUserEvent(Event event, bool rawInputMode) override;
	virtual void HandleSWEvents(std::vector<SWEvent>& swEvents) override;
	virtual void Update() override;
	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;

private:
	virtual void InitializeHotKeysActions() override;

private:
	MusicPlayer mMusicPlayer;
	ProgressBar mProgressBar;
};
