#include "Utility.hpp"

#include "ncurses.h"

#include <ncursesw/ncurses.h>
#undef OK

#include <codecvt>
#include <locale>
#include <stdexcept>

std::string getProjectDir()
{
	std::string projectName = "";
#ifdef PROJECT_DIR_CMAKE
	projectName = PROJECT_DIR_CMAKE;
#endif
	if (projectName.empty())
		throw std::logic_error("PROJECT_DIR_CMAKE IS UNDEFINED!");
	return std::move(projectName);
}

bool PrintwInLimitedX(const Vector2i& startPos, const std::string& textUTF_8, int xLimit,
	int startIndx, bool truncIfNotFit)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring text = converter.from_bytes(textUTF_8);

	if (xLimit <= 0)
		return false;

	if (startPos.x + text.size() > xLimit)
	{
		if (!truncIfNotFit)
		{
			return false;
		}

		// xLimit - 2 instead of xLimit, to place ".." at the end
		size_t availSize = xLimit - startPos.x - 2;
		mvaddwstr(startPos.y, startPos.x, text.substr(0, availSize).c_str());
		mvprintw(startPos.y, xLimit - 2, "..");
		return false;
	}
	else
	{
		mvprintw(startPos.y, startPos.x, textUTF_8.c_str());
		return true;
	}
}

void printwRectangle(const Recti& rect, bool hasSelection, int colorPair)
{
	int x1 = rect.left;
	int x2 = rect.left + rect.width;
	int y1 = rect.top;
	int y2 = rect.top + rect.height;

	if (hasSelection)
		attron(COLOR_PAIR(colorPair));

	mvhline(y1, x1, 0, x2 - x1);
	mvhline(y2, x1, 0, x2 - x1);
	mvvline(y1, x1, 0, y2 - y1);
	mvvline(y1, x2, 0, y2 - y1);
	mvaddch(y1, x1, ACS_ULCORNER);
	mvaddch(y2, x1, ACS_LLCORNER);
	mvaddch(y1, x2, ACS_URCORNER);
	mvaddch(y2, x2, ACS_LRCORNER);

	attroff(COLOR_PAIR(colorPair));
}

void clearArea(const Recti& rect)
{
	int x1 = rect.left;
	int x2 = rect.left + rect.width;
	int y1 = rect.top;
	int y2 = rect.top + rect.height;

	//	if (hasSelection)
	//		attron(COLOR_PAIR(1));

	//	mvhline(y1, x1, 0, x2 - x1);
	//	mvhline(y2, x1, 0, x2 - x1);
	//	mvvline(y1, x1, 0, y2 - y1);
	//	mvvline(y1, x2, 0, y2 - y1);
	//	mvaddch(y1, x1, ACS_ULCORNER);
	//	mvaddch(y2, x1, ACS_LLCORNER);
	//	mvaddch(y1, x2, ACS_URCORNER);
	//	mvaddch(y2, x2, ACS_LRCORNER);

	//	attroff(COLOR_PAIR(1));
	//	int x1 = rect.left;
	//	int x2 = rect.left + rect.width;
	//	int y1 = rect.top;
	//	int y2 = rect.top + rect.height;

	//	int x = rect.left;
	for (int y = y1 + 1; y < y2; ++y)
	{
		mvprintw(y, x1 + 1, std::string(rect.width - 1, ' ').c_str());
	}
}

void printwMsgBox(const std::string& msg, const Recti& availableSpace)
{
	int x1 = availableSpace.left;
	int y1 = availableSpace.top;
	int x2 = availableSpace.left + availableSpace.width;
	int y2 = availableSpace.top + availableSpace.height;

	int maxWidth = x2 - x1;
	int maxHeight = y2 - y1;

	// boundary coefficient, width to height
	float bestCoef = static_cast<float>(maxWidth) / static_cast<float>(maxHeight);
	int	  curWidth = 1;
	int	  curHeight = 1;
	float curCoef = curWidth / curHeight;
	auto  nCharFits = [&]() -> int { return curWidth * curHeight; };
	int	  msgSize = std::min(maxWidth * maxHeight, static_cast<int>(msg.size()));

	// Increase curWidth and curHeight, until we can fit our message(msg)
	// increase it so that curCoef will be close to bestCoef as much as possible
	while (nCharFits() < msgSize)
	{
		// if current coef less than needed, increase width, kae curCoef bigger
		if (curCoef < bestCoef && curWidth < maxWidth)
		{
			++curWidth;
		}
		// if current coef larger than needed, increase Height, make curCoef smaller
		else if (curCoef >= bestCoef && curHeight < maxHeight)
		{
			++curHeight;
		}
		curCoef = curWidth / curHeight;
	}

	// Drawing
	int yStart = y1 + (maxHeight - curHeight) / 2;
	int xStart = x1 + (maxWidth - curWidth) / 2;
	int msgIndx = 0;
	for (int h = 0; h < curHeight; ++h)
	{
		mvprintw(yStart + h, xStart, msg.substr(msgIndx, curWidth).c_str());
		msgIndx += curWidth;
	}
	Recti freeSpace{xStart - x1, yStart - y1, maxWidth - curWidth, maxHeight - curHeight};
	if (freeSpace.left > 0 && freeSpace.top > 0 && freeSpace.width > 0 &&
		freeSpace.height > 0)
	{
		printwRectangle(Recti{xStart - std::min(2, freeSpace.left), yStart - 1,
							curWidth + std::min(3, freeSpace.width), curHeight + 1},
			true, 2);
	}
}

ProgressBar::ProgressBar(char prgrsChar, int size)
	: prgrsChar(prgrsChar)
	, size(size)
{
}

std::string ProgressBar::GetProgressBar(int progressPercent, int xBoundary) const
{
	int tmpSize = size;
	if (xBoundary < size)
	{
		tmpSize = xBoundary;
	}

	std::string prgrsBar(tmpSize + 2, ' ');
	prgrsBar[0] = '[';
	prgrsBar[prgrsBar.size() - 1] = ']';

	int filledCells = tmpSize * progressPercent / 100;
	for (int i = 0; i < filledCells; ++i)
	{
		prgrsBar[i + 1] = prgrsChar;
	}
	return prgrsBar;
}

void printOptionsInLine(const Vector2i& startPos, int xAvailableSpace,
	const std::vector<std::string>& options, int selectionIndx, int colorPair)
{
	int optionsLenth = 0;
	for (auto& option : options)
	{
		optionsLenth += option.size();
	}

	int xBetweenTotal = xAvailableSpace - optionsLenth;
	int xBetween = xBetweenTotal / (options.size() + 1);

	int x = startPos.x;
	int y = startPos.y;

	for (int i = 0; i < options.size(); ++i)
	{
		auto& option = options[i];
		if (i == selectionIndx)
		{
			attron(COLOR_PAIR(colorPair) | A_REVERSE);
		}

		x += xBetween;
		mvprintw(y, x, option.c_str());

		if (i == selectionIndx)
		{
			//			attroff(COLOR_PAIR(colorPair));
			attroff(A_REVERSE | COLOR_PAIR(colorPair));
		}
	}
}
