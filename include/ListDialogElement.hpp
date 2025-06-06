#pragma once

#include "DialogElement.hpp"
#include "ElementSelector.hpp"

#include <vector>

/**
 * @brief The ListDialogElement class
 * - Consist of 3 visual elements:@n
 * 1. Text area @n
 * 2. List area @n
 * 3. Confirm area @n
 *
 * - Used to:@n
 * 1) Confirm list of elements (ReadOnly mode)@n
 * 2) Choose one element from the list (Once choice mode)@n
 * 3) Choose multiple elements from the list (Multiple choice mode)@n
 */
template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
class ListDialogElement : public DialogElement
{
public:
	enum class Type
	{
		ReadOnly = 0,
		OneChoice,
		MultipleChoice,
	};

	struct LocalBounds
	{
		Recti text;
		Recti list;
		Recti confirm;
	};

public:
	ListDialogElement(const Recti& bounds, std::string text,
		std::vector<ElementPtr> elements, Type type,
		std::vector<std::string>&& options = {CANCEL_OPTION, OK_OPTION});

	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;
	virtual void HandleRawInput(int rawInputChar) override;

	virtual void InputUp() override;
	virtual void InputDown() override;
	virtual void InputLeft() override;
	virtual void InputRight() override;
	virtual void InputEnter() override;
	virtual void InputTab() override;

private:
	Recti						mBounds;
	LocalBounds					mLBounds;
	HorizontalOptionChoice		mHorizontalChoice;
	std::string					mText;
	ElementSelector<ElementPtr> mFileSelector;
	Type						mType;
};

#include "ListDialogElement.tpp"
