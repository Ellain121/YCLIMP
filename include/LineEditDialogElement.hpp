#pragma once

#include "DialogElement.hpp"

/**
 * @brief The LineEditDialogElement class - Has line that user able to edit.@n
 * Goal of this Dialog is simply get some text line data from user@n
 */
class LineEditDialogElement : public DialogElement
{
public:
	/// bounds for some of the class visual objects
	struct LocalBounds
	{
		Recti text;
		Recti lineEdit;
		Recti confirm;
	};

public:
	LineEditDialogElement(const Recti& bounds, std::string text);

	virtual void Draw() const override;
	virtual void HandleResizeEvent(const Recti& newRect) override;
	virtual void HandleRawInput(int rawInputChar) override;

	virtual void InputUp() override;
	virtual void InputDown() override;
	virtual void InputLeft() override;
	virtual void InputRight() override;
	virtual void InputEnter() override;
	virtual void InputTab() override;

	const std::string& GetLineEditText() const;

private:
	Recti				   mBounds;
	LocalBounds			   mLocalBounds;
	std::string			   mText;
	HorizontalOptionChoice mHorizontalChoice;
	std::string			   mLineEditText;
};
