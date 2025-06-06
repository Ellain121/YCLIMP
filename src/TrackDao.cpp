#include "TrackDao.hpp"

#include "DatabaseManager.hpp"

namespace
{
const std::string& TRACKS_TABLE_NAME = "tracks";
const std::string& CREATE_TABLE_QUERY = "CREATE TABLE IF NOT EXISTS " +
										TRACKS_TABLE_NAME +
										" (id INTEGER PRIMARY KEY AUTOINCREMENT,"
										"playlist_id INT NOT NULL,"
										"name TEXT NOT NULL,"
										"from_type TEXT NOT NULL,"
										"location_id TEXT,"
										"fs_path TEXT NOT NULL,"
										"inode INT NOT NULL"
										")";
};	  // namespace

DBTrack::DBTrack(int id, int playlistId, std::string fromType, std::string locationId,
	std::string fsPath, std::string name, int inode)
	: id{id}
	, playlistId{playlistId}
	, fromType{std::move(fromType)}
	, locationId{std::move(locationId)}
	, fsPath{std::move(fsPath)}
	, name{std::move(name)}
	, inode{inode}
{
}

//-------------

TrackDao::TrackDao(SQLite::Database& database)
	: mDatabase{database}
{
}

void TrackDao::Init() const
{
	if (!mDatabase.tableExists(TRACKS_TABLE_NAME))
	{
		// table not exist, create one
		// tbh sql query already does create "if not exist query"
		SQLite::Statement createTableQuery{mDatabase, CREATE_TABLE_QUERY};
		DB::ExecAndDebug(createTableQuery);
	}
}

/// Fetch All tracks from specific playlist
std::vector<DBTrack> TrackDao::FetchAllFromPlaylist(int playlist_id) const
{
	std::vector<DBTrack> tracks;

	SQLite::Statement fetchAllFromPlaylistQuery{mDatabase,
		"SELECT * FROM " + TRACKS_TABLE_NAME + " WHERE playlist_id=(:playlist_id);"};
	fetchAllFromPlaylistQuery.bind(":playlist_id", playlist_id);
	while (fetchAllFromPlaylistQuery.executeStep())
	{
		auto& track = tracks.emplace_back();
		auto& query = fetchAllFromPlaylistQuery;

		track.id = query.getColumn(0).getInt();
		track.playlistId = playlist_id;
		track.name = query.getColumn(2).getString();
		track.fromType = query.getColumn(3).getString();
		track.locationId = query.getColumn(4).getString();
		track.fsPath = query.getColumn(5).getString();
		track.inode = query.getColumn(6).getInt();
	}
	return tracks;
}

int TrackDao::AddTrack(DBTrack track) const
{
}

std::vector<int> TrackDao::AddTracks(std::vector<DBTrack> tracks) const
{
	try
	{
		std::vector<int> orderIDS;
		orderIDS.reserve(tracks.size());
		for (auto& track : tracks)
		{
			SQLite::Statement addTrackQuery{mDatabase,
				"INSERT INTO " + TRACKS_TABLE_NAME +
					" (playlist_id, name, from_type, location_id, fs_path, inode) "
					"VALUES (:playlist_id, :name, :from_type, :location_id, "
					":fs_path, :inode);"};
			addTrackQuery.bind(":playlist_id", track.playlistId);
			addTrackQuery.bind(":name", track.name);
			addTrackQuery.bind(":from_type", track.fromType);
			addTrackQuery.bind(":location_id", track.locationId);
			addTrackQuery.bind(":fs_path", track.fsPath);
			addTrackQuery.bind(":inode", track.inode);
			DB::ExecAndDebug(addTrackQuery);
			int rowID = mDatabase.getLastInsertRowid();
			orderIDS.push_back(rowID);
		}
		return orderIDS;
	}
	catch (const std::exception& err)
	{
		throw 1337;
	}
}
