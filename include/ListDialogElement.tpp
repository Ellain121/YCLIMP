#pragma once

#include "ListDialogElement.hpp"

namespace
{
template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
typename ListDialogElement<ElementPtr>::LocalBounds CalculateLocalBounds(
	const Recti& newBounds)
{
	typename ListDialogElement<ElementPtr>::LocalBounds lBounds;
	auto&												textBounds = lBounds.text;
	auto&												listBounds = lBounds.list;
	auto&												confirmBounds = lBounds.confirm;
	textBounds.left = newBounds.left;
	textBounds.width = newBounds.width;
	listBounds.left = newBounds.left;
	listBounds.width = newBounds.width;
	confirmBounds.left = newBounds.left;
	confirmBounds.width = newBounds.width;

	textBounds.top = newBounds.top;
	textBounds.height = std::max(static_cast<int>(newBounds.height * 0.1), 1);

	listBounds.top = newBounds.top + textBounds.height + 1;
	listBounds.height = std::max(static_cast<int>(newBounds.height * 0.8), 3);

	confirmBounds.top = listBounds.top + listBounds.height + 1;
	confirmBounds.height = std::max(static_cast<int>(newBounds.height * 0.1), 1);

	return lBounds;
}
}	 // namespace

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
ListDialogElement<ElementPtr>::ListDialogElement(const Recti& bounds, std::string text,
	std::vector<ElementPtr> elements, Type type, std::vector<std::string>&& options)
	: DialogElement{bounds}
	, mBounds{bounds}
	, mLBounds{CalculateLocalBounds<ElementPtr>(bounds)}
	, mHorizontalChoice{std::move(options), mLBounds.confirm}
	, mText{std::move(text)}
	, mFileSelector{mLBounds.list}
	, mType{type}
{
	mFileSelector.Load(std::move(elements), true);
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::Draw() const
{
	if (IsFinished() == true)
		return;

	clearArea(mBounds);
	printwMsgBox(mText, mLBounds.text);
	mFileSelector.Draw();
	mHorizontalChoice.Draw();
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::HandleResizeEvent(const Recti& newRect)
{
	if (IsFinished() == true)
		return;

	mLBounds = CalculateLocalBounds<ElementPtr>(newRect);
	mFileSelector.HandleResize(mLBounds.list);
	mHorizontalChoice.HandleResizeEvent(mLBounds.confirm);
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::HandleRawInput(int rawInputChar)
{
	if (IsFinished() == true)
		return;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::InputUp()
{
	if (IsFinished() == true)
		return;

	mFileSelector.SelectUp();
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::InputDown()
{
	if (IsFinished() == true)
		return;

	mFileSelector.SelectDown();
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::InputLeft()
{
	if (IsFinished() == true)
		return;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::InputRight()
{
	if (IsFinished() == true)
		return;

	if (mType == Type::ReadOnly)
		return;

	if (mType == Type::OneChoice)
	{
		mFileSelector.ClearAllLastingSelections();
	}

	mFileSelector.LastingSelectOnCurrent();
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::InputEnter()
{
	if (IsFinished() == true)
		return;

	mHorizontalChoice.ConfirmCurrentChoice();
	SetFinished(std::string{mHorizontalChoice.GetFinalChoice()});

	auto selected = mFileSelector.GetSelected();
	if (selected.has_value())
	{
		SetData(mFileSelector.GetSelected().value()->name);
	}
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ListDialogElement<ElementPtr>::InputTab()
{
	if (IsFinished() == true)
		return;

	mHorizontalChoice.SelectNext();
}
