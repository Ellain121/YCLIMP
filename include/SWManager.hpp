#pragma once

#include "Context.hpp"
#include "HotKeyComponent.hpp"
#include "SWHolder.hpp"
#include "SubWindow.hpp"

#include <functional>
#include <unordered_map>
#include <vector>

/**
 * @brief SubWindowManager is a SubWindow's and SWHolder's manager
 *
 * - There are multiple predefined SubWindow holders with each unique SWHolderType enum @n
 * - Each SWHolder has hardcoded  SubWindows, each SWHolder can hold one or many SubWindows @n
 * - SWHolder doesn't own SubWindows. It's only responsible for Positions, Size, Resize events and what SW to display @n
 * - SWManager owns and manages SubWindows @n
 */
class SWManager
{
private:
	using SWFactoryTable =
		std::unordered_map<ESWTypes, std::function<SubWindow::Ptr(Recti, SWHolderType)>>;

public:
	SWManager(const Recti& rect, MainWindow& mainWindow);

	template <std::derived_from<SubWindow> T>
	void RegisterSW(ESWTypes swType);

	void InitStartupState();

	void AddSWEvent(SWEvent swEvent);
	void EventLoopIterationStart();
	void HandleUserEvent(Event event);
	void HandleSWEvents();
	void Update();
	void Draw() const;
	void HandleResizeEvent(Recti newRect);
	void UpdatePosElementsSize();

	void setRawInputMode(bool rawInputMode);

private:
	SubWindow::Ptr CreateSubWindow(ESWTypes swType, SWHolderType holderType);

	void SelectTopInfoHolder();
	void SelectMainHolder();
	void SelectAdditionalHolder();

	void InitializeHotKeyActions();
	void InitializePosElementsSize();

	void DeselectAll();

	SWHolder&	  GetSWHolder(SWHolderType holderType);
	inline size_t GetSWIndex(ESWTypes swType) const;
	Recti		  GetBoundsFromPosType(PositionType posType) const;

private:
	Recti			mRect;
	Context			mContext;
	HotKeyComponent mHotKeyComponent;
	bool			mRawInputMode;

	Recti mTopElementRect;
	Recti mOneMidElementRect;
	Recti mLeftElementRect;
	Recti mRightElementRect;
	Recti mCentralElementRect;

	std::array<SWHolder, 4>		mSWHolders;
	std::vector<SubWindow::Ptr> mSubWindows;
	SWFactoryTable				mFactories;

	std::vector<SWEvent> mSWEvents;
};

// register to be able to create it from SWFactoryTable
template <std::derived_from<SubWindow> T>
void SWManager::RegisterSW(ESWTypes swType)
{
	mFactories[swType] = [this](
							 Recti bounds, SWHolderType inHolderType = SWHolderType::None)
	{ return std::make_unique<T>(bounds, mContext, inHolderType); };
}
