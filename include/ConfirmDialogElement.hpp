#pragma once

#include "DialogElement.hpp"

/**
 * @brief The ConfirmDialogElement class - Used to Confirm some information.@n
 * Usually the only data/input that changes is mHorizontalChoice.
 */
class ConfirmDialogElement : public DialogElement
{
public:
	/// bounds for some of the class visual objects
	struct LocalBounds
	{
		Recti text;
		Recti dataText;
		Recti confirm;
	};

public:
	ConfirmDialogElement(
		const std::string& text, const std::string& confirmText, const Recti& bounds);

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
	Recti				   mBounds;
	LocalBounds			   mLocalBounds;
	HorizontalOptionChoice mHorizontalChoice;
	std::string			   mText;
	std::string			   mConfirmText;
};
