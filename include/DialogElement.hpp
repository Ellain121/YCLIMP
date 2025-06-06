#pragma once

#include "Primitives.hpp"

#include <string>
#include <variant>
#include <vector>

const std::string CANCEL_OPTION = "Cancel";
const std::string OK_OPTION = "Ok";
const std::string ADD_TO_NEW_PLAYLIST_OPTION = "AddToNewPlaylist";

/**
 * @brief The HorizontalOptionChoice class - part of the DialogElement.@n
 * Most or all of the dialogs has this horizontal option choice at the bottom of the window.
 *
 *  Logic:
 * - 1) Select specific option@n
 * - 2) Confirm your selection by ConfirmCurrentChoice@n
 * - 3) Selection mechanism is off. Able to get your final choice selection@n
 */
class HorizontalOptionChoice
{
public:
	HorizontalOptionChoice(std::vector<std::string> options, const Recti& bounds);

	void Draw() const;
	void SelectNext();
	void ConfirmCurrentChoice();
	void HandleResizeEvent(const Recti& newBounds);

	std::string_view GetFinalChoice() const;
	std::size_t		 GetFinalChoiceIndx() const;

private:
	std::size_t				 mCurrentSelection;
	std::vector<std::string> mOptions;
	Recti					 mBounds;
	bool					 mFinalChoice;
};

/**
 * @brief The DialogElement class - Abstract class.@n
 * Visual element designed to process some user input in a dialog format.
 *
 * - When it's done, it's not terminated immediately. SetFinished is used.@n
 * Derived classes utilises it to turn them off(ex.: if finished not draw) untill they're cleaned.
 */
class DialogElement
{
public:
	DialogElement(const Recti& bounds);

	virtual void Draw() const = 0;
	virtual void HandleResizeEvent(const Recti& newRect) = 0;
	virtual void HandleRawInput(int rawInputChar) = 0;

	virtual void			   SetFinished(std::string finishedStatus);
	virtual bool			   IsFinished() const;
	virtual const std::string& GetFinishedStatus() const;

	void SetData(const std::variant<std::vector<std::size_t>, std::string>& data);
	virtual std::variant<std::vector<std::size_t>, std::string> GetData() const;

	virtual void InputUp() = 0;
	virtual void InputDown() = 0;
	virtual void InputLeft() = 0;
	virtual void InputRight() = 0;
	virtual void InputEnter() = 0;
	virtual void InputTab() = 0;

private:
	bool												mIsFinished;
	std::string											mFinishedStatus;
	std::variant<std::vector<std::size_t>, std::string> mData;
};
