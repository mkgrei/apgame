# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

from .socket import Socket

USER_COMMAND_JOIN_USER = 0
USER_COMMAND_EXIT = 1

class UserClient(object):
    
    def __init__(self, socket):
        self._socket = socket
    
    def joinUser(self, name):
        self._socket.sendInt32(USER_COMMAND_JOIN_USER)
        self._socket.sendString(name)
        error = self._socket.recieveInt32()
        return error
    
    def exit(self, name):
        self._socket.sendInt32(USER_COMMAND_EXIT)
        error = self._socket.recieveInt32()
        return error

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='user client')
    parser.add_argument('--host', dest='host', required=True, type=str)
    parser.add_argument('--port', dest='port', required=True, type=int)
    
    args = parser.parse_args()
    socket = Socket(host=args.host, port=args.port)
    user_client = UserClient(socket)

    from IPython import embed
    embed()
