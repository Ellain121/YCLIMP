from yandex_music import Client, Playlist, TrackShort
from yandex_music import Track, StationTracksResult
from typing import List, Tuple, Dict, Any
from radio import Radio
from server import *
from utility import *
from Token import TOKEN
import logging
#

logging.basicConfig(
    format=u'%(filename)s[line:%(lineno)d]# %(levelname)-4s [%(asctime)s]  %(message)s', level=logging.DEBUG, filename='python.log')


class StaticDirs(Enum):
    ROOT = 0
    PLAYLISTS = 1
    RADIO = 2
    FAVORITES = 3


dirs = {"0": StaticDirs.ROOT, "1": StaticDirs.PLAYLISTS,
        "2": StaticDirs.RADIO, "3": StaticDirs.FAVORITES}


class YaClient:
    def __init__(self, __server):
        self._server: Server = __server
        self._client: Client = Client(TOKEN).init()
        # _tracks: List[TrackShort] = []
        # _user_playlists: List[Playlist]
        self._radio: Radio = None

    def run(self):
        while True:
            cmd = self._server.wait_cmd()
            self._execute_cmd(cmd)

    def _execute_cmd(self, cmd: CMD):
        logging.info(cmd.cmd)
        if cmd.cmd == Command.GET_DIR_FILES:
            id: str = cmd.arg
            return_dirs = self._get_dir_files(id)
            self._server.send_dir_files(return_dirs)

        elif cmd.cmd == Command.GET_TRACK:
            id: str = cmd.arg
            track_bytes = self._get_track(id)
            self._server.send_track_bytes(track_bytes)

        elif cmd.cmd == Command.RADIO_NEXT:
            self._radio_next()

    def _get_dir_files(self, id: str):
        return_list: List[List[str]] = []
        if id in dirs:
            dir_id = dirs[id]
            if dir_id == StaticDirs.ROOT:
                return_list.append("folder")  # shitcode, yeah
                return_list.append(("Playlists", "1"))
                return_list.append(("Radio", "2"))
            elif dir_id == StaticDirs.PLAYLISTS:
                return_list.append("folder")  # shitcode, yeah
                playlists = self._client.users_playlists_list()
                for playlist in playlists:
                    return_list.append((playlist.title,  str(playlist.kind)))
                return_list.append(
                    (("Favorites", str(StaticDirs.FAVORITES.value))))

            elif dir_id == StaticDirs.RADIO:

                pass

            elif dir_id == StaticDirs.FAVORITES:
                return_list.append("audio")  # shitcode, yeah
                favorite_tracks = self._client.users_likes_tracks()
                fav_tracks = favorite_tracks.fetch_tracks()
                for f_track in fav_tracks:
                    file_name = f_track.title + ":" + \
                        ' '.join(map(str, f_track.artistsName()))
                    logging.debug(file_name, f_track.id)
                    return_list.append((file_name, str(f_track.id)))

        else:
            # specific playlist id
            # Get specific playlist tracks
            # Warning code quality is dubious
            return_list.append("audio")  # shitcode, yeah
            playlist_id: str = id
            playlist = self._client.users_playlists(playlist_id)
            sh_tracks = playlist.fetch_tracks()
            track_ids = []
            for sh_track in sh_tracks:
                track_ids.append(sh_track.id)

            tracks = self._client.tracks(track_ids)
            for track in tracks:
                file_name = track.title + ":" + \
                    ' '.join(map(str, track.artistsName()))
                logging.debug(file_name, track.id)
                return_list.append((file_name, str(track.id)))
            ###################

        return return_list

    def _get_track_bytes(self, cmd: CMD):
        id = cmd.arg
        assert id in self._tracks, "No track with such id"
        sh_track: TrackShort = self._tracks[id]
        track: Track = sh_track.fetch_track()
        bytes = track.download_bytes()
        return bytes

    def _radio_start(self):
        self._radio = Radio(self._client)
        station_id = 'user:onyourwave'
        station_from = "potato"
        first_track = self._radio.start_radio(station_id, station_from)
        # todo redo, probably first track is lost
        print('[Radio] First track is:', first_track.title)
        print("Waiting...")

    def _radio_next(self):
        track = self._radio.play_next()
        file_name = ','.join(map(str, track.artistsName())) + \
            ": " + track.title
        print("Getting bytes... " + file_name)
        bytes = (file_name + "\r\n").encode() + \
            track.download_bytes()
        print("Sending... " + file_name)
        self._server.send_track_bytes(bytes)

    def _get_user_playlists(self):
        user_playlists: List[Playlist] = self._client.user_playlists_list()
        return_list: List[List[str, str]] = []

        for playlist in user_playlists:
            id = str(playlist.uid + ":" + str(playlist.kind))
            return_list.append([id, playlist.title])
        # for id, playlist in enumerate(self._user_playlists):
            # return_list.append([id, playlist.title])

        return return_list

    def _get_user_pl_tracks(self, pl_id: str):
        user_pl: Playlist = self._client.playlists_list(pl_id)[0]
        return_list: List[List[id, str]] = []
        for sh_track in user_pl.fetch_tracks():
            track = sh_track.fetch_track()
            file_name = ','.join(map(str, track.artistsName())) + \
                ": " + track.title
            print("List:", file_name)

            track_id = sh_track.id
            return_list.append([track_id, file_name])

        return return_list

    def _get_track(self, tr_id: str | int):
        track = self._client.tracks(tr_id)[0]
        file_name = ','.join(map(str, track.artistsName())) + \
            ": " + track.title
        track_bytes = (file_name + '\r\n').encode() + track.download_bytes()
        return track_bytes
