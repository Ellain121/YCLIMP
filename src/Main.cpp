#include "Application.hpp"
#include "ncurses.h"
#undef OK

#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <thread>

std::string exec(const char* cmd)
{
    char        buffer[128];
    std::string result = "";
    FILE*       pipe = popen(cmd, "r");
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

bool isTokenCorrect(const std::string& token)
{
    std::regex pattern(R"(^TOKEN *= *['\"][a-zA-Z0-9_-]+['\"])");
    if (std::regex_match(token, pattern))
    {
        std::cerr << "Okay token!" << std::endl;
        return true;
    }
    return false;
}

bool isTokenOkay()
{
    std::ifstream inputFile("../python_yaclient/Token.py");
    if (inputFile.is_open())
    {
        std::cerr << "Open" << std::endl;
        std::string line;
        std::getline(inputFile, line);
        if (isTokenCorrect(line))
        {
            std::cerr << "Token file is good!" << std::endl;
            return true;
        }
    }
    return false;
}

void CheckPythonYaToken()
{
    bool bToken = isTokenOkay();
    if (!bToken)
    {
        std::string token;
        do
        {
            std::cout << "Paste yandex token: ";
            std::cin >> token;
        } while (!isTokenCorrect("TOKEN='" + token + "'"));
        std::ofstream tokenFile("../python_yaclient/Token.py");
        assert(tokenFile.is_open() == true);
        tokenFile << ("TOKEN='" + token + "'");
        tokenFile.close();
    }
}

void StartPythonClient()
{
    exec("python3 ../python_yaclient/main.py > file.txt 2>&1");
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
    // temp crutch btw
    // [Init]
    CheckPythonYaToken();

    startNcurses();
    std::thread pythonYaClientThread(StartPythonClient);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // [/Init]

    Application app;
    app.Run();

    pythonYaClientThread.join();
    endNcurses();
}
