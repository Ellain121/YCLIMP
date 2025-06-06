#pragma once

struct Context;
class MainWindow;
class SWManager;

class FSClient;
class YandexClient;
class AppClient;

class DatabaseManager;

struct Context
{
public:
	Context(MainWindow& mWindow, SWManager& swManager, FSClient& fsClient,
		YandexClient& yandexClient, AppClient& appClient, DatabaseManager& dbManager);

public:
	MainWindow&		 mainWindow;
	SWManager&		 swManager;
	FSClient&		 fsClient;
	YandexClient&	 yaClient;
	AppClient&		 appClient;
	DatabaseManager& dbManager;
};
