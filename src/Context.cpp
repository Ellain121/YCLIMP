#include "Context.hpp"

#include "DatabaseManager.hpp"
#include "FileManager.hpp"
#include "MainWindow.hpp"
#include "SWManager.hpp"

Context::Context(MainWindow& mWindow, SWManager& swManager, FSClient& fsClient,
	YandexClient& yandexClient, AppClient& appClient, DatabaseManager& dbManager)
	: mainWindow{mWindow}
	, swManager{swManager}
	, fsClient{fsClient}
	, yaClient{yandexClient}
	, appClient{appClient}
	, dbManager{dbManager}
{
}
