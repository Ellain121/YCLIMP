#pragma once

#include "ElementSelector.hpp"
#include "FileManager.hpp"
#include "SubWindow.hpp"

/**
 * @brief The PlayerSW class;
 * Player SubWindow class is responsible for selecting tracks to play now
 */
class PlayerSW : public SubWindow
{
public:
	PlayerSW(const Recti& bounds, Context context,
		SWHolderType inHolderType = SWHolderType::None);

	virtual void HandleUserEvent(Event event, bool rawInputMode) override;
	virtual void HandleSWEvents(std::vector<SWEvent>& swEvents) override;
	virtual void Update() override;
	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;

private:
	virtual void InitializeHotKeysActions() override;
	void		 DrawElemSelectorInfo() const;

private:
	// Step 1: try
	// bool                    mStandartSelector;
	// Mode         mMode;
	FileSelector mFileSelector;
};
