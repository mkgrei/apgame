# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals
from builtins import bytes, str

import socket
import struct

class Socket(object):
    _socket = None
    _buffer = None
    def __init__(self, host, port):
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._socket.connect((host, port))
        self._buffer = ''
    
    def __del__(self):
        self._socket.close()
    
    def sendInt32(self, val):
        self._send(struct.pack('<i', val))
    
    def sendUInt64(self, val):
        self._send(struct.pack('<Q', val))
    
    def sendString(self, val):
        self.sendUInt64(len(val))
        self._send(struct.pack('{}s'.format(len(val)), val.encode('utf8')))
   
    def recieveBool(self):
        self._recieve(1)
        return struct.unpack('?', self._consume(1))
   
    def recieveInt32(self):
        self._recieve(4)
        return struct.unpack('<i', self._consume(4))
    
    def recieveUInt64(self):
        self._recieve(8)
        return struct.unpack('<Q', self._consume(8))
    
    def recieveString(self):
        size = self.recieveUInt64()
        return self.unpack('{}s'.format(size), self._consume(size))
    
    def _consume(self, length):
        data = self._buffer[:length]
        self._buffer = self._buffer[length:]
        return data
    
    def _send(self, data):
        print('send {} bytes'.format(len(data)))
        total = 0
        while total < len(data):
            size = self._socket.send(data[total:])
            if size == 0:
                raise RuntimeError('connection closed')
            total += size
    
    def _recieve(self, recv_size):
        print('recieve{} bytes'.format(recv_size))
        while len(self._buffer) < recv_size:
            segment = self._socket.recv(4096)
            if len(segment) == 0:
                raise RuntimeError('connection closed')
            self._buffet += segment
