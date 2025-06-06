#include "SubWindow.hpp"

#include "SWManager.hpp"
#include "Utility.hpp"

#include <stdexcept>

SWEvent::SWEvent(ESWTypes evTarget, ESWEventAction evAction,
	const std::variant<std::vector<File_Ptr>, File_Ptr, std::string>& data,
	const std::string&												  data_2)
	: target{evTarget}
	, action{evAction}
	, data{data}
	, data_2{data_2}
{
}

std::unordered_map<ESWTypes, std::string> SubWindow::mTypeToNameTable = {
	{ESWTypes::StartWindow, "StartWindowSW"}, {ESWTypes::Player, "PlayerSW"},
	{ESWTypes::PlayerStatus, "PlayerStatusSW"}, {ESWTypes::Settings, "SettingsSW"},
	{ESWTypes::FileExplorer, "FileExplorerSW"}, {ESWTypes::LocalFiles, "LocalFilesSW"},
	{ESWTypes::YandexFiles, "YandexFilesSW"}, {ESWTypes::AppFiles, "AppFilesSW"},
	{ESWTypes::Dialog, "DialogSW"}};

const std::string& SubWindow::ConvertToSWName(ESWTypes swType)
{
	if (mTypeToNameTable.find(swType) == mTypeToNameTable.end())
	{
		throw std::logic_error(
			"SubWindow::ConvertToSWName: Cannot convert\
                                                swType to appropriate string");
	}

	return mTypeToNameTable[swType];
}

SubWindow::SubWindow(
	ESWTypes swType, const Recti& bounds, Context context, SWHolderType inHolderType)
	: mType{swType}
	, mBounds{bounds}
	, mContext{context}
	, mInHolderType{inHolderType}
	, mHotKeyComponent{ConvertToSWName(swType)}
	, mSelected{false}
	, mIsVisible{false}
	, mHighDrawPriority{false}
{
}

void SubWindow::EventLoopIterationStart()
{
}

void SubWindow::HandleUserEvent(Event event, bool rawInputMode)
{
	if (!rawInputMode)
		mHotKeyComponent.HandleUserEvent(event, HasSelection());
}

void SubWindow::HandleResizeEvent(const Recti& newRect)
{
	mBounds = newRect;
}

void SubWindow::Reload()
{
}

void SubWindow::DrawBoundsRectangle(const std::string& rectName) const
{
	printwRectangle(mBounds, HasSelection());
	if (!rectName.empty())
	{
		PrintwInLimitedX(Vector2i{mBounds.left + 5, mBounds.top}, rectName,
			mBounds.left + 5 + rectName.size());
	}
}

void SubWindow::Select()
{
	mSelected = true;
}

void SubWindow::Deselect()
{
	mSelected = false;
}

bool SubWindow::IsVisible() const
{
	return mIsVisible;
}

void SubWindow::SetVisible(bool visibleValue)
{
	mIsVisible = visibleValue;
}

void SubWindow::SetHighDrawPriority(bool priority)
{
	mHighDrawPriority = priority;
}

bool SubWindow::GetDrawPriority() const
{
	return mHighDrawPriority;
}

SWHolderType SubWindow::GetHolderType() const
{
	return mInHolderType;
}

bool SubWindow::HasSelection() const
{
	return mSelected;
}

Context SubWindow::GetContext()
{
	return mContext;
}

const Recti& SubWindow::GetBounds() const
{
	return mBounds;
}

const std::string& SubWindow::GetName() const
{
	return ConvertToSWName(mType);
}

ESWTypes SubWindow::GetSWType() const
{
	return mType;
}
