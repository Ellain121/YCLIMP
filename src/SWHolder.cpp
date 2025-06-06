#include "SWHolder.hpp"

#include <cassert>
#include <limits>

namespace
{
std::string ConvertToHolderName(SWHolderType holderType)
{
	if (holderType == SWHolderType::TopInfo)
	{
		return "TopInfoHolder";
	}
	else if (holderType == SWHolderType::Main)
	{
		return "MainHolder";
	}
	else if (holderType == SWHolderType::Additional)
	{
		return "AdditionalHolder";
	}
	else if (holderType == SWHolderType::Central)
	{
		return "CentralHolder";
	}
	else
	{
		assert(("invalid holderType!!", false));
	}
}
};	  // namespace

SWHolder::SWHolder(SWHolderType holderType, Recti bounds, PositionType posType)
	//	: mHotKeyComponent{ConvertToHolderName(holderType)}
	: mHolderType{holderType}
	, mPosType{posType}
	, mBounds{bounds}
	, mSubWindows{}
	, mSelectedSW{nullptr}
	, mIsHolderSelected{false}
{
}

void SWHolder::HoldSubWindow(SubWindow::Ptr subWindow)
{
	bool bVisible = true;
	if (mHolderType == SWHolderType::Central)
	{
		subWindow->SetHighDrawPriority(true);
		bVisible = false;
	}
	mSubWindows.push_back(subWindow);
	// first sw to be added will be potentionally selected
	if (mSelectedSW == nullptr)
	{
		mSelectedSW = std::move(subWindow);
		mSelectedSW->SetVisible(bVisible);
	}
}

void SWHolder::HandleResizeEvent(Recti newRect)
{
	mBounds = newRect;
	for (auto& subWindow : mSubWindows)
	{
		subWindow->HandleResizeEvent(newRect);
	}
}

void SWHolder::SetNewPosNBounds(PositionType posType, Recti bounds)
{
	mPosType = posType;
	mBounds = bounds;
	for (auto& subWindow : mSubWindows)
	{
		subWindow->HandleResizeEvent(bounds);
	}
}

void SWHolder::EnableAndSelect()
{
	assert(("mSelectedSW is nullptr!", mSelectedSW != nullptr));

	mSelectedSW->SetVisible(true);
	mSelectedSW->Select();
}

void SWHolder::DisableAndDeselect()
{
	assert(("mSelectedSW is nullptr!", mSelectedSW != nullptr));

	mSelectedSW->SetVisible(false);
	mSelectedSW->Deselect();
}

void SWHolder::Select()
{
	assert(("mSelectedSW is nullptr!", mSelectedSW != nullptr));

	mIsHolderSelected = true;
	mSelectedSW->Select();
}

void SWHolder::Deselect()
{
	//	assert(("mSelectedSW is nullptr!", mSelectedSW != nullptr));
	if (mSelectedSW)
	{
		mSelectedSW->Deselect();
		mIsHolderSelected = false;
	}
}

void SWHolder::SelectSubWindow(ESWTypes swType)
{
	if (mSelectedSW != nullptr)
	{
		mSelectedSW->Deselect();
	}

	bool found = false;
	for (auto& subWindow : mSubWindows)
	{
		if (subWindow->GetSWType() == swType)
		{
			mSelectedSW = subWindow;
			subWindow->SetVisible(true);
			subWindow->Select();
			found = true;
		}
		else
		{
			subWindow->SetVisible(false);
			subWindow->Deselect();
		}
	}
	assert(("SubWindow with such swType not found!", found == true));
}

bool SWHolder::IsSelected() const
{
	return mIsHolderSelected;
}

bool SWHolder::IsVisible() const
{
	if (mSelectedSW)
	{
		return mSelectedSW->IsVisible();
	}
	return false;
}

PositionType SWHolder::GetPosType() const
{
	return mPosType;
}

const Recti& SWHolder::GetBounds() const
{
	return mBounds;
}
