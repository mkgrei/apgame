# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

import json

from .reversi import Reversi

from logging import getLogger
log = getLogger(__name__)

class GameManager(object):
    _game_dispatch_table = {
        'reversi': Reversi
    }
    def __init__(self):
        pass

    def process_command(self, data):
        log.debug('process_command: data = {}'.format(json.dumps(data, indent=4)))
        game_name = data['game_name'] 
