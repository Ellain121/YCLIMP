#include "DatabaseManager.hpp"

#include "Constants.hpp"
#include "Debug.hpp"

namespace DB
{

void ExecAndDebug(SQLite::Statement& query)
{
	try
	{
		int rowsAffected = query.exec();
		DatabaseManager::DebugQuery(query.getQuery(), rowsAffected);
	}
	catch (const std::exception& exception)
	{
		Debug::Print("SQL exception: " + std::string(exception.what()));
	}
}
}	 // namespace DB

DatabaseManager& DatabaseManager::Instance()
{
	static DatabaseManager singletonDB{DATABASE_FILENAME};
	return singletonDB;
}

void DatabaseManager::DebugQuery(const std::string& query, int modifiedRows)
{
	Debug::Print("QUERY: " + query + "|Modified Rows: " + std::to_string(modifiedRows));
}
// clang-format off
DatabaseManager::DatabaseManager(const std::string& path)
try
	: mDatabase{path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}
	, trackDao{mDatabase}
	, playlistDao{mDatabase}
{
	trackDao.Init();
	playlistDao.Init();
}
// clang-format on
catch (const std::exception& e)
{
	Debug::Print(std::string{"EXCEPTION: "} + e.what());
}
