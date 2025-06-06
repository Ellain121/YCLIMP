#include "LineEditDialogElement.hpp"

#include "Utility.hpp"

namespace
{
LineEditDialogElement::LocalBounds CalculateLocalBounds(const Recti& newBounds)
{
	LineEditDialogElement::LocalBounds lBounds;
	auto&							   textBounds = lBounds.text;
	auto&							   lineEdit = lBounds.lineEdit;
	auto&							   confirmBounds = lBounds.confirm;
	textBounds.left = newBounds.left;
	textBounds.width = newBounds.width;
	lineEdit.left = newBounds.left + 1;
	lineEdit.width = newBounds.width;
	confirmBounds.left = newBounds.left;
	confirmBounds.width = newBounds.width;

	textBounds.top = newBounds.top;
	textBounds.height = std::max(static_cast<int>(newBounds.height * 0.4), 1);

	lineEdit.top = newBounds.top + textBounds.height + 1;
	lineEdit.height = std::max(static_cast<int>(newBounds.height * 0.4), 1);

	confirmBounds.top = lineEdit.top + lineEdit.height + 1;
	confirmBounds.height = std::max(static_cast<int>(newBounds.height * 0.2), 1);

	return lBounds;
}
}	 // namespace

LineEditDialogElement::LineEditDialogElement(const Recti& bounds, std::string text)
	: DialogElement{bounds}
	, mBounds{bounds}
	, mLocalBounds{CalculateLocalBounds(bounds)}
	, mText{std::move(text)}
	, mHorizontalChoice{{"Cancel", "Ok"}, mLocalBounds.confirm}
{
}

void LineEditDialogElement::Draw() const
{
	if (IsFinished() == true)
		return;

	clearArea(mBounds);
	printwMsgBox(mText, mLocalBounds.text);
	PrintwInLimitedX({mLocalBounds.lineEdit.left, mLocalBounds.lineEdit.top},
		"->" + mLineEditText + "<-",
		mLocalBounds.lineEdit.left + mLocalBounds.lineEdit.width);
	mHorizontalChoice.Draw();
}

void LineEditDialogElement::HandleResizeEvent(const Recti& newRect)
{
	if (IsFinished() == true)
		return;

	mLocalBounds = CalculateLocalBounds(newRect);
	mHorizontalChoice.HandleResizeEvent(mLocalBounds.confirm);
}

void LineEditDialogElement::HandleRawInput(int rawKey)
{
	if (IsFinished() == true)
		return;

	bool isKeyHotKey = rawKey == 9 || rawKey == 13;
	bool isKeyValid = (rawKey >= 'a' && rawKey <= 'z') ||
					  (rawKey >= 'A' && rawKey <= 'Z') || rawKey == ' ' ||
					  rawKey == KEY_BACKSPACE;
	// hotkeys are deactivated in awinput mode
	if (isKeyHotKey)
	{
		if (rawKey == 9)
			InputTab();
		else if (rawKey == 13)
			InputEnter();

		return;
	}

	if (!isKeyValid)
	{
		return;
	}

	if (rawKey == KEY_BACKSPACE)
	{
		if (mLineEditText.size() > 0)
			mLineEditText.pop_back();
	}
	else
	{
		mLineEditText.push_back(static_cast<char>(rawKey));
	}
}

void LineEditDialogElement::InputUp()
{
	if (IsFinished() == true)
		return;
}

void LineEditDialogElement::InputDown()
{
	if (IsFinished() == true)
		return;
}

void LineEditDialogElement::InputLeft()
{
	if (IsFinished() == true)
		return;
}

void LineEditDialogElement::InputRight()
{
	if (IsFinished() == true)
		return;
}

void LineEditDialogElement::InputEnter()
{
	if (IsFinished() == true)
		return;

	mHorizontalChoice.ConfirmCurrentChoice();
	SetFinished(std::string{mHorizontalChoice.GetFinalChoice()});
	SetData(mLineEditText);
}

void LineEditDialogElement::InputTab()
{
	if (IsFinished() == true)
		return;

	mHorizontalChoice.SelectNext();
}

const std::string& LineEditDialogElement::GetLineEditText() const
{
	return mLineEditText;
}
