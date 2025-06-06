#include "PlaylistDao.hpp"

#include "DatabaseManager.hpp"
#include "Debug.hpp"

namespace
{
const std::string& PLAYLISTS_TABLE_NAME = "playlists";

const std::string& CREATE_PLAYLISTS_TABLE_QUERY =
	"CREATE TABLE IF NOT EXISTS " + PLAYLISTS_TABLE_NAME +
	" (id INTEGER PRIMARY KEY AUTOINCREMENT,"
	"name TEXT NOT NULL UNIQUE);";

std::string GetOrderDBName(int id)
{
	return "id_" + std::to_string(id) + "_order";
}

std::string GetBasedOnDBName(int id)
{
	return "id_" + std::to_string(id) + "_based_on";
}
}	 // namespace

DBPlaylist::DBPlaylist(int id, const std::string& name)
	: id{id}
	, name{name}
	, trackOrder{}
	, basedOnList{}
{
}

DBPlaylist::DBPlaylist(int id, const std::string& name, std::vector<int> trackOrder,
	std::vector<BasedOn> basedOnList)
	: id{id}
	, name{name}
	, trackOrder{std::move(trackOrder)}
	, basedOnList{std::move(basedOnList)}
{
}
//---------------------------

PlaylistDao::PlaylistDao(SQLite::Database& database)
	: mDatabase{database}
{
}

void PlaylistDao::Init() const
{
	if (!mDatabase.tableExists(PLAYLISTS_TABLE_NAME))
	{
		// table not exist, create one
		// tbh sql query already does create "if not exists query" but whatever
		SQLite::Statement createTableQuery{mDatabase, CREATE_PLAYLISTS_TABLE_QUERY};
		DB::ExecAndDebug(createTableQuery);
	}
}

int PlaylistDao::AddPlaylist(const std::string& name) const
{
	SQLite::Statement newPlaylistQuery{
		mDatabase, "INSERT INTO " + PLAYLISTS_TABLE_NAME + " (name) VALUES (:name);"};
	newPlaylistQuery.bind(":name", name);
	DB::ExecAndDebug(newPlaylistQuery);
	int				  rowId = mDatabase.getLastInsertRowid();
	SQLite::Statement createBasedOnDB{mDatabase, "CREATE TABLE IF NOT EXISTS " +
													 GetBasedOnDBName(rowId) + " " +
													 "(from_type TEXT NOT NULL,"
													 "location_id TEXT NOT NULL,"
													 "fs_path TEXT NOT NULL,"
													 "name TEXT NOT NULL"
													 ");"};
	SQLite::Statement createOrderDB{
		mDatabase, "CREATE TABLE IF NOT EXISTS " + GetOrderDBName(rowId) + " " +
					   "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
					   "track_id INT NOT NULL"
					   ");"};
	DB::ExecAndDebug(createBasedOnDB);
	DB::ExecAndDebug(createOrderDB);

	return rowId;
}

/**
 * @brief If playlist with such name existed, return it's id
 * @param name - playlist name
 * @return playlist id (existed or newly created)
 */
int PlaylistDao::AddNewOrGetPlaylist(const std::string& name) const
{
	/*** GET PlaylistID if ANY ***/
	SQLite::Statement checkPlaylistQuery{
		mDatabase, "SELECT * FROM " + PLAYLISTS_TABLE_NAME + " WHERE name=(:name);"};
	checkPlaylistQuery.bind(":name", name);
	int playlistID = -1;

	try
	{
		if (checkPlaylistQuery.executeStep())
		{
			playlistID = checkPlaylistQuery.getColumn(0).getInt();
		}
		DatabaseManager::DebugQuery(checkPlaylistQuery.getQuery(), 1);
	}
	catch (const std::exception& exception)
	{
		Debug::Print("SQL exception: " + std::string(exception.what()));
	}
	/*** ************* ***/

	// if not such playlsit
	if (playlistID == -1)
	{
		// then create and return it's id
		return AddPlaylist(name);
	}
	else
	{
		// then return existed playlist id
		return playlistID;
	}
}

void PlaylistDao::RenamePlaylist(int playlistId, const std::string& newName) const
{
	SQLite::Statement renamePlaylist{
		mDatabase, "UPDATE " + PLAYLISTS_TABLE_NAME + " SET name=:name WHERE id=:id;"};
	renamePlaylist.bind(":id", playlistId);
	DB::ExecAndDebug(renamePlaylist);
}

std::vector<DBPlaylist> PlaylistDao::FetchAll() const
{
	std::vector<DBPlaylist> playlists;
	try
	{
		SQLite::Statement getAllQuery{
			mDatabase, "SELECT * FROM " + PLAYLISTS_TABLE_NAME + ";"};
		while (getAllQuery.executeStep())
		{
			int			id = getAllQuery.getColumn(0).getInt();
			std::string name = getAllQuery.getColumn(1).getString();
			DBPlaylist	playlist{id, name};
			playlists.push_back(std::move(playlist));
			// TODO later
		}
	}
	catch (const std::exception& e)
	{
		Debug::Print(std::string("EXCEPTION: ") + e.what());
		throw e;
	}
	return playlists;
}

void PlaylistDao::AddTracksToPlaylist(int playlistId, std::vector<int> trackIds) const
{
	for (int trackID : trackIds)
	{
		SQLite::Statement addTrackIDQuery{
			mDatabase, "INSERT INTO " + GetOrderDBName(playlistId) +
						   " (track_id) VALUES (:track_id);"};
		addTrackIDQuery.bind(":track_id", trackID);
		DB::ExecAndDebug(addTrackIDQuery);
	}
}
