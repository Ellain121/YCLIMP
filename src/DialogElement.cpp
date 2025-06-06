#include "DialogElement.hpp"

#include "Utility.hpp"

HorizontalOptionChoice::HorizontalOptionChoice(
	std::vector<std::string> options, const Recti& bounds)
	: mBounds{bounds}
	, mOptions{std::move(options)}
	, mFinalChoice{false}
	, mCurrentSelection{0}
{
}

void HorizontalOptionChoice::Draw() const
{
	// writeen badly, but fine for now
	int y = mBounds.top + mBounds.height / 2;
	int x = mBounds.left;
	//	printwRectangle(mBounds);
	printOptionsInLine({x, y}, mBounds.width, mOptions, mCurrentSelection, 4);
	//
}

/// Next option in a loop (right selection)
void HorizontalOptionChoice::SelectNext()
{
	if (mFinalChoice)
		return;

	mCurrentSelection++;
	if (mCurrentSelection >= mOptions.size())
	{
		mCurrentSelection = 0;
	}
}

void HorizontalOptionChoice::ConfirmCurrentChoice()
{
	mFinalChoice = true;
}

void HorizontalOptionChoice::HandleResizeEvent(const Recti& newBounds)
{
	mBounds = newBounds;
}

std::string_view HorizontalOptionChoice::GetFinalChoice() const
{
	return mOptions[mCurrentSelection];
}

std::size_t HorizontalOptionChoice::GetFinalChoiceIndx() const
{
	return mCurrentSelection;
}

DialogElement::DialogElement(const Recti& bounds)
	: mIsFinished{false}
	, mFinishedStatus{}
	, mData{}
{
}

void DialogElement::SetFinished(std::string finishedStatus)
{
	mIsFinished = true;
	mFinishedStatus = std::move(finishedStatus);
}

bool DialogElement::IsFinished() const
{
	return mIsFinished;
}

const std::string& DialogElement::GetFinishedStatus() const
{
	return mFinishedStatus;
}

void DialogElement::SetData(
	const std::variant<std::vector<std::size_t>, std::string>& data)
{
	mData = data;
}

std::variant<std::vector<std::size_t>, std::string> DialogElement::GetData() const
{
	return mData;
}
