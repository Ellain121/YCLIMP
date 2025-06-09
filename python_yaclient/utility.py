import socket
import re
from enum import Enum
from typing import List, Tuple, Dict, Any

import logging
#


logging.basicConfig(
    format=u'%(filename)s[line:%(lineno)d]# %(levelname)-4s [%(asctime)s]  %(message)s', level=logging.DEBUG, filename='python.log')


leftovers_queue = ""


class Command(Enum):
    GET_DIR_FILES = 0
    GET_TRACK = 1
    RADIO_NEXT = 2


cmd_table = {"GET_DIR_FILES": Command.GET_DIR_FILES,
             "GET_TRACK": Command.GET_TRACK, "RADIO_NEXT": Command.RADIO_NEXT}


class CMD:
    def __init__(self, _cmd, _arg=0):
        self.cmd: Command = _cmd
        self.arg: Any = _arg


def extract_cmd(msg: str) -> CMD:
    cmd_list = re.findall(r'\w+|\s', msg)
    assert len(cmd_list) > 0
    logging.info("msg:|", msg)
    cmd_str = cmd_list[0]
    logging.info("here:|", cmd_str, "|", cmd_list)
    assert cmd_str in cmd_table, "Unknown command!"
    cmd = cmd_table.get(cmd_str)

    if cmd == Command.GET_DIR_FILES or cmd == Command.GET_TRACK:
        arg = str(cmd_list[2])
        return CMD(cmd, arg)

    return CMD(cmd, -1)


def recv(user_socket: socket, delim):
    global leftovers_queue
    dataStr = ""
    if len(leftovers_queue) > 0:
        if '\r\n' in leftovers_queue:
            dataStr = leftovers_queue
        else:
            data = user_socket.recv(1024)
            dataStr = leftovers_queue + data.decode()

    else:
        data = user_socket.recv(1024)
        dataStr = data.decode()

    msg = dataStr[:dataStr.index('\r\n')]
    leftovers = dataStr[dataStr.index('\r\n') + len('\r\n'):]
    logging.info("Message:|", msg)
    logging.info("Leftovers:", leftovers, " size: ", len(leftovers))
    leftovers_queue = leftovers
    return msg
