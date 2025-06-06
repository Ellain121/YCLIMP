from typing import List, Tuple, Dict, Any
from utility import *
import socket
import logging

PORT, HOST_IP = 5002, '127.0.0.1'
EOF = 'E!O$F'

logging.basicConfig(
    format=u'%(filename)s[line:%(lineno)d]# %(levelname)-4s [%(asctime)s]  %(message)s', level=logging.DEBUG, filename='python.log')


class Server:
    def __init__(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((HOST_IP, PORT))
        self.socket.listen()
        logging.info("Listening...")
        self.conn, self.addr = self.socket.accept()
        logging.info('Connected by', self.addr)

    def wait_cmd(self):
        msg = recv(self.conn, '\r\n')
        return extract_cmd(msg)

    # send_dir_files = send_any_files # cause shitcode!
    def send_dir_files(self, list: List[List[str]]):
        # bytes = 'folder\r\n'
        bytes = list[0] + "\r\n"
        for dir_el in list[1:]:  # start from second
            entry = dir_el[0] + ',' + dir_el[1] + '\r\n'
            bytes += entry
        self.conn.sendall(bytes.encode() + EOF.encode())
        logging.debug("len", len(list))
        logging.debug(bytes)
        logging.debug("dir_files sent!")

    def send_user_playlist_meta(self, list: List[List[str]]):
        pass  # todo later

    def send_track_list(self, track_list):
        bytes = ''
        for track_el in track_list:
            entry = track_el[0] + ',' + track_el[1] + '\r\n'
            bytes += entry
        self.conn.sendall(bytes.encode() + EOF.encode())
        logging.debug("track_list sent!")

    def send_track_bytes(self, bytes):
        self.conn.sendall(bytes + EOF.encode())
        logging.debug("song_bytes sent!")
