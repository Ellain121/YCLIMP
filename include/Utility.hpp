/**
 * @file
 * @brief Utility function list
 */

#pragma once

#include "Primitives.hpp"

#include <string>
#include <vector>

enum class SWHolderType
{
	None = -1,
	TopInfo = 0,
	Main,
	Additional,
	Central
};

std::string getProjectDir();

/**
 * @brief
 * Printw line in specified x boundary.
 * if line is larger than x boundary.
 * then truncate the line.
 *
 * @param startPos cordinates on the screen
 * @param text string to print(draw)
 * @param xLimit available space in x axis
 * @param startIndx start printing from indx
 * @param truncIfNotFit truncate text if it cannot be fitted fully
 * @return if line can be fitted return true, else false
 */
bool PrintwInLimitedX(const Vector2i& startPos, const std::string& text, int xLimit,
	int startIndx = 0, bool truncIfNotFit = true);

/**
 * printw(draw) rectangular bounds.
 * @param rect rectangle to printw(draw)
 * @param hasSelection if true uses colorPair(1 by default) to draw rectangle
 * @param colorPair if hasSelection true, use specifiedd colorPair
 */
void printwRectangle(const Recti& rect, bool hasSelection = false, int colorPair = 1);

/**
 * @brief clearArea - clears specified rect, rect itself is not included,@n
 * only area inside rectangle without bounds
 * @param rect - rect to clear
 */
void clearArea(const Recti& rect);

/**
 * uses printwRectangle to draw rectangle and prints in there a message.
 * Note! actual box(bounds) are dynamicly created according to the message size.
 * If message is too big to be fit with bounds, bounds are ignored.
 * If message still to big to be feet, it will be truncated(without "...").
 * @param msg - message to printw(draw)
 * @param availableSpace - max boundaries
 */
void printwMsgBox(const std::string& msg, const Recti& availableSpace);

/**
 * raw function, HorizontalOptionChoice uses it
 * It's written badly, but it's fine for now
 */
void printOptionsInLine(const Vector2i& startPos, int xAvailableSpace,
	const std::vector<std::string>& options, int selectionIndx = -1, int colorPair = 1);

struct ProgressBar
{
	ProgressBar(char prgrsChar, int size);
	std::string GetProgressBar(int progressPercent, int xBoundary = __INT_MAX__) const;

	int	 size;
	char prgrsChar;
};
