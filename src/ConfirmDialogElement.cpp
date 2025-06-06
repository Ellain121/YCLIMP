#include "ConfirmDialogElement.hpp"

#include "Utility.hpp"

namespace
{
ConfirmDialogElement::LocalBounds CalculateLocalBounds(const Recti& newBounds)
{
	ConfirmDialogElement::LocalBounds lBounds;
	auto&							  textBounds = lBounds.text;
	auto&							  dataText = lBounds.dataText;
	auto&							  confirmBounds = lBounds.confirm;
	textBounds.left = newBounds.left;
	textBounds.width = newBounds.width;
	dataText.left = newBounds.left;
	dataText.width = newBounds.width;
	confirmBounds.left = newBounds.left;
	confirmBounds.width = newBounds.width;

	textBounds.top = newBounds.top;
	textBounds.height = std::max(static_cast<int>(newBounds.height * 0.1), 1);

	dataText.top = newBounds.top + textBounds.height + 1;
	dataText.height = std::max(static_cast<int>(newBounds.height * 0.8), 1);

	confirmBounds.top = dataText.top + dataText.height + 1;
	confirmBounds.height = std::max(static_cast<int>(newBounds.height * 0.1), 1);

	return lBounds;
}
}	 // namespace

ConfirmDialogElement::ConfirmDialogElement(
	const std::string& text, const std::string& confirmText, const Recti& bounds)
	: DialogElement{bounds}
	, mBounds{bounds}
	, mLocalBounds{CalculateLocalBounds(bounds)}
	, mHorizontalChoice{{"Cancel", "Ok"}, mLocalBounds.confirm}
	, mText{text}
	, mConfirmText{confirmText}
{
}

void ConfirmDialogElement::Draw() const
{
	if (IsFinished() == true)
		return;

	clearArea(mBounds);
	printwMsgBox(mText, mLocalBounds.text);
	printwMsgBox(mConfirmText, mLocalBounds.dataText);
	mHorizontalChoice.Draw();
}

void ConfirmDialogElement::HandleResizeEvent(const Recti& newRect)
{
	if (IsFinished() == true)
		return;

	mLocalBounds = CalculateLocalBounds(newRect);
	mHorizontalChoice.HandleResizeEvent(mLocalBounds.confirm);
}

void ConfirmDialogElement::HandleRawInput(int rawInputChar)
{
	if (IsFinished() == true)
		return;
}

void ConfirmDialogElement::InputUp()
{
	if (IsFinished() == true)
		return;
}

void ConfirmDialogElement::InputDown()
{
	if (IsFinished() == true)
		return;
}

void ConfirmDialogElement::InputLeft()
{
	if (IsFinished() == true)
		return;
}

void ConfirmDialogElement::InputRight()
{
	if (IsFinished() == true)
		return;
}

void ConfirmDialogElement::InputEnter()
{
	if (IsFinished() == true)
		return;

	mHorizontalChoice.ConfirmCurrentChoice();
	SetFinished(std::string{mHorizontalChoice.GetFinalChoice()});
}

void ConfirmDialogElement::InputTab()
{
	if (IsFinished() == true)
		return;

	mHorizontalChoice.SelectNext();
}
