#include "Application.hpp"
#include "ncurses.h"
#undef OK

#include <exception>
#include <thread>

std::string exec(const char* cmd)
{
	char		buffer[128];
	std::string result = "";
	FILE*		pipe = popen(cmd, "r");
	if (!pipe)
	{
		throw std::runtime_error("popen failed");
	}
	try
	{
		while (fgets(buffer, sizeof(buffer), pipe) != NULL)
		{
			result += buffer;
		}
	}
	catch (...)
	{
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
}

void StartPythonClient()
{
	exec("python3 ../python_yaclient/main.py");
}

void startNcurses()
{
	// UTF-8
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	raw();
	start_color();
	curs_set(0);
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	nonl();
	halfdelay(1.f);
	init_pair(1, COLOR_GREEN, 0);
	init_pair(2, COLOR_YELLOW, 0);
	init_pair(3, COLOR_MAGENTA, 0);
	init_pair(4, COLOR_CYAN, 0);
}

void endNcurses()
{
	endwin();
}

int main()
{
	// [Init]
	startNcurses();
	std::thread pythonYaClientThread(StartPythonClient);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	// [/Init]

	Application app;
	app.Run();

	pythonYaClientThread.join();
	endNcurses();
}
