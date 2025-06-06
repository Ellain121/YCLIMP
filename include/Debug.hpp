#pragma once

#include <string>

/**
 * @brief The Debug class - Draw messages with ncurses or print them in debug file
 *
 * With ncurses there is no use with stdout and stderr@n
 * - All the members are static, can be used from anywhere
 */
class Debug
{
public:
	static void DrawMsg(const std::string& msg);
	static void Draw(int posY, int posX);
	static void Print(const std::string& msg, const std::string& filename = "");
	static void Print(const std::wstring& msg);
	static void Init();

private:
	static std::string message;
};
