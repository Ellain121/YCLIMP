#include "SWManager.hpp"

#include "DatabaseManager.hpp"
#include "FileManager.hpp"
#include "MainWindow.hpp"

#include <cassert>
#include <limits>

namespace
{

constexpr size_t NotFound = std::numeric_limits<size_t>::max();
const float		 centHolderCoeff = 0.7f;
Recti			 MultiplyRect(const Recti& rect, float coeff)
{
	if (coeff < 1.0f)
	{
		Recti newRect = rect;
		int	  width = rect.width * coeff;
		int	  height = rect.height * coeff;
		int	  widthDiff = rect.width - width;
		int	  heightDiff = rect.height - height;

		newRect.left += widthDiff / 2;
		newRect.width = width;

		newRect.top += heightDiff / 2;
		newRect.height = height;

		return newRect;
	}
	assert(("This function is not fully written! Only works for coeff < 1.0", false));
}

}	 // namespace

SWManager::SWManager(const Recti& rect, MainWindow& mainWindow)
	: mRect{rect}
	, mContext{mainWindow, *this, FSClient::Instance(), YandexClient::Instance(),
		  AppClient::Instance(), DatabaseManager::Instance()}
	, mHotKeyComponent{"SWManager"}
	, mRawInputMode{false}
	, mTopElementRect{Recti{0, 0, mRect.width - 1, 5}}
	, mOneMidElementRect{Recti{0, mTopElementRect.top + mTopElementRect.height + 1,
		  mRect.width - 1,
		  mRect.height - mTopElementRect.top - mTopElementRect.height - 2}}
	, mLeftElementRect{Recti{0 + mOneMidElementRect.left, 0 + mOneMidElementRect.top,
		  mOneMidElementRect.width / 2, mOneMidElementRect.height}}
	, mRightElementRect{Recti{0 + mOneMidElementRect.left + mLeftElementRect.width + 1,
		  0 + mOneMidElementRect.top, (mOneMidElementRect.width - 1) / 2,
		  mOneMidElementRect.height}}
	, mCentralElementRect{MultiplyRect(mRect, centHolderCoeff)}
	, mSWHolders{SWHolder{SWHolderType::TopInfo, mTopElementRect, PositionType::Top},
		  SWHolder{SWHolderType::Main, mLeftElementRect, PositionType::Left},
		  SWHolder{SWHolderType::Additional, mRightElementRect, PositionType::Right},
		  SWHolder{SWHolderType::Central, mCentralElementRect, PositionType::Central}}
	, mSWEvents{}
{
	InitializeHotKeyActions();
	InitializePosElementsSize();
}

/**
 * @brief SWManager::InitStartupState
 *
 * Add required startups SubWindows@n
 * Add them immediately, without waiting for next loop or smth similar@n
 */
void SWManager::InitStartupState()
{
	CreateSubWindow(ESWTypes::PlayerStatus, SWHolderType::TopInfo);
	CreateSubWindow(ESWTypes::Player, SWHolderType::Main);
	CreateSubWindow(ESWTypes::LocalFiles, SWHolderType::Additional);
	CreateSubWindow(ESWTypes::YandexFiles, SWHolderType::Additional);
	CreateSubWindow(ESWTypes::AppFiles, SWHolderType::Additional);
	CreateSubWindow(ESWTypes::Dialog, SWHolderType::Central);
}

void SWManager::InitializeHotKeyActions()
{
	mHotKeyComponent.InsertBindingAction(
		std::make_pair("select_top", [this]() { this->SelectTopInfoHolder(); }));
	mHotKeyComponent.InsertBindingAction(
		std::make_pair("select_mid_or_left", [this]() { this->SelectMainHolder(); }));
	mHotKeyComponent.InsertBindingAction(
		std::make_pair("select_right", [this]() { this->SelectAdditionalHolder(); }));

	mHotKeyComponent.InsertBindingAction(std::make_pair("local_files_sw",
		[this]()
		{
			size_t indx = GetSWIndex(ESWTypes::LocalFiles);
			assert(("invalid sw index!", indx != NotFound));
			auto&		 subWindow = mSubWindows[indx];
			SWHolderType holderType = subWindow->GetHolderType();
			auto&		 swHolder = GetSWHolder(holderType);

			swHolder.SelectSubWindow(subWindow->GetSWType());
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("yandex_files_sw",
		[this]()
		{
			size_t indx = GetSWIndex(ESWTypes::YandexFiles);
			assert(("invalid sw index!", indx != NotFound));
			auto&		 subWindow = mSubWindows[indx];
			SWHolderType holderType = subWindow->GetHolderType();
			auto&		 swHolder = GetSWHolder(holderType);

			swHolder.SelectSubWindow(subWindow->GetSWType());
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("app_files_sw",
		[this]()
		{
			size_t indx = GetSWIndex(ESWTypes::AppFiles);
			assert(("invalid sw index!", indx != NotFound));
			auto&		 subWindow = mSubWindows[indx];
			SWHolderType holderType = subWindow->GetHolderType();
			auto&		 swHolder = GetSWHolder(holderType);

			subWindow->Reload();
			swHolder.SelectSubWindow(subWindow->GetSWType());
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("player_sw",
		[this]()
		{
			size_t indx = GetSWIndex(ESWTypes::Player);
			assert(("invalid sw index!", indx != NotFound));
			auto&		 subWindow = mSubWindows[indx];
			SWHolderType holderType = subWindow->GetHolderType();
			auto&		 swHolder = GetSWHolder(holderType);

			subWindow->Reload();
			swHolder.SelectSubWindow(subWindow->GetSWType());
		}));
	mHotKeyComponent.InsertBindingAction(std::make_pair("player_status_sw",
		[this]()
		{
			size_t indx = GetSWIndex(ESWTypes::PlayerStatus);
			assert(("invalid sw index!", indx != NotFound));
			auto&		 subWindow = mSubWindows[indx];
			SWHolderType holderType = subWindow->GetHolderType();
			auto&		 swHolder = GetSWHolder(holderType);

			subWindow->Reload();
			swHolder.SelectSubWindow(subWindow->GetSWType());
		}));
}

void SWManager::InitializePosElementsSize()
{
	UpdatePosElementsSize();
}

void SWManager::AddSWEvent(SWEvent swEvent)
{
	mSWEvents.push_back(std::move(swEvent));
}

void SWManager::EventLoopIterationStart()
{
	for (auto& subWindow : mSubWindows)
	{
		subWindow->EventLoopIterationStart();
	}
}

void SWManager::HandleUserEvent(Event event)
{
	// if not in raw input mode, process SWManager hotkeys
	if (!mRawInputMode)
	{
		// local binding make no sense for swmanager, it cannot be selected or deselected
		mHotKeyComponent.HandleUserEvent(event, false);
	}

	// SubWindows Events:
	for (auto& subWindow : mSubWindows)
	{
		subWindow->HandleUserEvent(event, mRawInputMode);
	}
}

void SWManager::HandleSWEvents()
{
	// Process events related to SWManager
	std::vector<SWEvent> eventsToProcess = std::move(mSWEvents);
	for (auto& swEvent : eventsToProcess)
	{
		// if None, means it's target is SWManager
		if (swEvent.target == ESWTypes::None)
		{
			switch (swEvent.action)
			{
				case ESWEventAction::EnableCentralHolder:
				{
					auto& swHolder = GetSWHolder(SWHolderType::Central);
					DeselectAll();
					swHolder.EnableAndSelect();
					SWEvent newEvent{ESWTypes::Dialog,
						ESWEventAction::ChooseActionWithSelectedElements, swEvent.data};
					mSWEvents.push_back(std::move(newEvent));
				}
				break;

				case ESWEventAction::DisableCentralHolder:
				{
					auto& swHolder = GetSWHolder(SWHolderType::Central);
					swHolder.DisableAndDeselect();
				}
				break;

				default:	// will not be processed + unknown event
				{
					mSWEvents.push_back(swEvent);
					throw std::logic_error("Error!");
				}
				break;
			}
		}
		else	// will not be processed
		{
			mSWEvents.push_back(swEvent);
		}
	}

	// process events, that targeted not at SWManager
	for (auto& subWindow : mSubWindows)
	{
		subWindow->HandleSWEvents(mSWEvents);
	}
}

void SWManager::Update()
{
	for (auto& subWindow : mSubWindows)
	{
		subWindow->Update();
	}
}

void SWManager::Draw() const
{
	// if SW doesn't have a high draw priority, we draw it first, potentially on
	// background
	for (auto& subWindow : mSubWindows)
	{
		if (subWindow->GetDrawPriority() == false)
			subWindow->Draw();
	}

	// if SW has high draw priority, we draw it afterwards, so it will be in the front
	for (auto& subWindow : mSubWindows)
	{
		if (subWindow->GetDrawPriority() == true)
			subWindow->Draw();
	}
}

void SWManager::HandleResizeEvent(Recti newRect)
{
	mRect = std::move(newRect);

	UpdatePosElementsSize();

	for (auto& swHolder : mSWHolders)
	{
		PositionType posType = swHolder.GetPosType();
		Recti		 holderRect = GetBoundsFromPosType(posType);
		swHolder.HandleResizeEvent(holderRect);
	}
	//	for (auto& subWindow : mSubWindows)
	//	{
	//		EPositionType posType = subWindow->GetPosType();
	//		subWindow->HandleResizeEvent(GetPosTypeBounds(posType));
	//	}
}

/**
 * @brief SWManager::UpdatePosElementsSize
 *
 * 4 positional rectangles: Top, OneMid, Left, Right@n
 * Update each one rectangle based on mRect, mRect is fullapplication window size@n
 * Usually used after resize to update it's positions@n
 */
void SWManager::UpdatePosElementsSize()
{
	mTopElementRect = Recti{0, 0, mRect.width - 1, 5};

	mOneMidElementRect = Recti{0, mTopElementRect.top + mTopElementRect.height + 1,
		mRect.width - 1, mRect.height - mTopElementRect.top - mTopElementRect.height - 2};

	mLeftElementRect = Recti{0 + mOneMidElementRect.left, 0 + mOneMidElementRect.top,
		mOneMidElementRect.width / 2, mOneMidElementRect.height};

	mRightElementRect = Recti{0 + mOneMidElementRect.left + mLeftElementRect.width + 1,
		0 + mOneMidElementRect.top, (mOneMidElementRect.width - 1) / 2,
		mOneMidElementRect.height};

	mCentralElementRect = MultiplyRect(mRect, centHolderCoeff);
}

void SWManager::SelectTopInfoHolder()
{
	DeselectAll();
	auto& swHolder = GetSWHolder(SWHolderType::TopInfo);
	swHolder.Select();
}

void SWManager::SelectMainHolder()
{
	DeselectAll();
	auto& swHolder = GetSWHolder(SWHolderType::Main);
	swHolder.Select();
}

void SWManager::SelectAdditionalHolder()
{
	DeselectAll();
	auto& swHolder = GetSWHolder(SWHolderType::Additional);
	swHolder.Select();
}

/// create subwindow immediately, place it in swHolder and mSubWindows@n
/// return shared_ptr to this subwindow
SubWindow::Ptr SWManager::CreateSubWindow(ESWTypes swType, SWHolderType holderType)
{
	assert(("SWManager::CreateSubWindow Error: swType not found in mFactories",
		mFactories.find(swType) != mFactories.end()));

	SWHolder&	 swHolder = GetSWHolder(holderType);
	const Recti& holderBounds = swHolder.GetBounds();

	SubWindow::Ptr subWindow = mFactories[swType](holderBounds, holderType);
	swHolder.HoldSubWindow(subWindow);
	mSubWindows.push_back(subWindow);

	return subWindow;
}

SWHolder& SWManager::GetSWHolder(SWHolderType holderType)
{
	int indx = static_cast<int>(holderType);
	assert(("holderType convertion to indx gone wrong!",
		indx >= 0 && indx < mSWHolders.size()));

	return mSWHolders[indx];
}

/**
 * @brief SWManager::GetSWIndex
 * @param swType
 * @return size_t indx if found;
 * maximum size_t value if not found
 */
size_t SWManager::GetSWIndex(ESWTypes swType) const
{
	for (size_t i{0}; i < mSubWindows.size(); ++i)
	{
		if (mSubWindows[i]->GetSWType() == swType)
		{
			return i;
		}
	}
	return NotFound;
}

Recti SWManager::GetBoundsFromPosType(PositionType posType) const
{
	if (posType == PositionType::Top)
	{
		return mTopElementRect;
	}
	else if (posType == PositionType::Left)
	{
		return mLeftElementRect;
	}
	else if (posType == PositionType::MidOnly)
	{
		return mOneMidElementRect;
	}
	else if (posType == PositionType::Right)
	{
		return mRightElementRect;
	}
	else
	{
		assert(("Wrong posType in GetBoundsFromPosType", false));
	}
}

void SWManager::DeselectAll()
{
	for (auto& swHolder : mSWHolders)
	{
		swHolder.Deselect();
	}
}

/// if rawInputMode enabled for class, it's hotkeys(hotkeycomponent) won't be processed
void SWManager::setRawInputMode(bool rawInputMode)
{
	mRawInputMode = rawInputMode;
}
