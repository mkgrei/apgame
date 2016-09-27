# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

from .agent import Agent

import json
import os

class Game(object):

    _agent_list = []

    def __init__(self, config):
        self._agent_path_list = config['agent_path_list']
        self._agent_root_dir = os.environ['APGAME_AGENT_ROOT_DIR']

    def _launch_game(self):
        for agent_path in self._agent_path_list:
            self._launch_agent(agent_path)

    def _launch_agent(self, agent_path):
        agent = Agent(agent_path)
        agent.start_docker()
        self._agent_list.append(agent)

    def __enter__(self):
        self._launch_game()

    def __exit__(self, exception_type, exception_value, traceback):
        for agent in self._agent_list:
            agent.kill_docker()

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='apgame.game')
    parser.add_argument('--config-path', dest='config_path', required=True)

    args = parser.parse_args()
    with open(args.config_path, 'r') as fp:
        config = json.load(fp)

    with Game(config) as fp:
        from IPython import embed
        embed()
