#include "Debug.hpp"

#include "Utility.hpp"
#include "ncurses.h"
#undef OK

#include <chrono>
#include <fstream>

namespace chrn = std::chrono;

std::string Debug::message = "";

/// Set message to draw
void Debug::DrawMsg(const std::string& msg)
{
	message = msg;
}

/// Method that actually draws stuff with ncurses
void Debug::Draw(int posY, int posX)
{
	mvprintw(posY, posX, "%s", message.c_str());
	// message.clear();
}

/// Method that prints message to the specified file
void Debug::Print(const std::string& msg, const std::string& filename)
{
	if (filename.empty())
	{
		std::ofstream outFile{getProjectDir() + "/log.txt", std::ios::app};
		outFile << "[" << chrn::system_clock::now() << "]: " << msg << std::endl;
	}
	else
	{
		std::ofstream outFile{getProjectDir() + "/" + filename, std::ios::app};
		outFile << "[" << chrn::system_clock::now() << "]: " << msg << std::endl;
	}
}

/// Method that prints to default log file
void Debug::Print(const std::wstring& msg)
{
	std::wofstream outFile{getProjectDir() + "/log.txt", std::ios::app};
	outFile << "[" << chrn::system_clock::now() << "]: " << L"here: " << std::flush;
	outFile << msg << std::endl;
}

void Debug::Init()
{
	// std::ofstream file{getProjectDir() + "/log.txt"};
}
