#pragma once

#include "PlaylistDao.hpp"
#include "TrackDao.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

namespace DB
{
void ExecAndDebug(SQLite::Statement& query);
}	 // namespace DB

/**
 * @brief The DatabaseManager class
 *
 * Singleton@n
 * Has public class members, each one does specific SQL queries@n
 * The only reason for that is not to clutter this class@n
 * I don't catch almost all exceptions in this class and it's members.@n
 * If something will throw. it means it's a bug or DB problem that should be fixed.
 */
class DatabaseManager
{
public:
	static DatabaseManager& Instance();

	static void DebugQuery(const std::string& query, int modifiedRows);

private:
	DatabaseManager(const std::string& path);
	DatabaseManager& operator=(const DatabaseManager& rhs) = delete;

private:
	SQLite::Database mDatabase;

public:
	const TrackDao	  trackDao;
	const PlaylistDao playlistDao;
};
