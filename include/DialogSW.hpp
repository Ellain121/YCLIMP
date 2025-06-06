#pragma once

#include "SubWindow.hpp"

#include <DialogBundle.hpp>
#include <memory>

/**
 * @brief The DialogSW class - operates with Dialog Bundles
 *
 * @warning For now it's hardcoded to NewElementsDialogBundle, since it's the only DialogBundle for now
 *
 * - 1) Operates with DialogBundle
 * - 2) DialogBundle in it's turn, operates with specific sequence of DialogElements
 * - Usually Invisible, but if the program needs some input from user, just makes it temporary visible, untill it's done@n
 */
class DialogSW : public SubWindow
{
public:
	DialogSW(const Recti& bounds, Context context,
		SWHolderType inHolderType = SWHolderType::Central);

	virtual void EventLoopIterationStart() override;
	virtual void HandleUserEvent(Event event, bool rawInputMode) override;
	virtual void HandleSWEvents(std::vector<SWEvent>& swEvents) override;
	virtual void Update() override;
	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;

private:
	virtual void InitializeHotKeysActions() override;

private:
	std::unique_ptr<DialogBundle> mDialogBundle;
};
