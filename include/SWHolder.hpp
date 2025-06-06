#pragma once

#include "HotKeyComponent.hpp"
#include "SubWindow.hpp"
#include "Utility.hpp"

#include <memory>
#include <vector>

// each swHolder belongs to some position
enum class PositionType
{
	None = -1,
	Top,
	MidOnly,	//
	Left,
	Right,
	Central,
};

/**
 * @brief The SWHolder class does not own SubWindows
 * It's only responsible for Positions, Size, Resize events and what SubWindow to display
 *
 * @warning Despite the fact, that SWHolder is responsible for Size/Resize/Pos, it's not responsible for drawing anything at all.@n
 * BoundingRectangles usually drawn by SubWindows.@n
 * Once again, this class has no Draw method and simply unable to display stuff.@n
 *
 * - Has bindings/hotkeys
 * - Created right from the start
 * - Some of SWHolders will be hidden if not needed
 * - Selectable, if selected, local bindings will be active
 */
class SWHolder
{
public:
	SWHolder(SWHolderType holderType, Recti bounds, PositionType posType);

	void HoldSubWindow(SubWindow::Ptr subWindow);
	void HandleResizeEvent(Recti newRect);
	void UpdateRectSize(Recti newRect);

	void SetNewPosNBounds(PositionType posType, Recti bounds);
	void EnableAndSelect();
	void DisableAndDeselect();
	void Select();
	void Deselect();

	void SelectSubWindow(ESWTypes swType);

	bool		 IsSelected() const;
	bool		 IsVisible() const;
	PositionType GetPosType() const;
	const Recti& GetBounds() const;

private:
	SWHolderType				mHolderType;
	PositionType				mPosType;
	Recti						mBounds;
	std::vector<SubWindow::Ptr> mSubWindows;
	SubWindow::Ptr				mSelectedSW;
	bool						mIsHolderSelected;
};
