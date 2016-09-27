# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

import json
import os
import subprocess

from logging import basicConfig, getLogger, getLevelName

log = getLogger(__name__)

class Agent(object):

    _user_name = None
    _agent_name = None
    _agent_root_dir = None
    _docker_process = None
    _docker_image = None

    def __init__(self, agent_name):
        self._agent_root_dir = os.environ['APGAME_AGENT_ROOT_DIR']
        self._agent_name = agent_name
        self._agent_dir = os.path.join(self._agent_root_dir, self._agent_name)
        self._agent_source_dir = os.path.join(self._agent_dir, 'source')
        self._agent_build_dir = os.path.join(self._agent_dir, 'build')
        self._agent_config_file = os.path.join(self._agent_dir, 'agent.config')
        self._load_config()

    def _load_config(self):
        with open(self._agent_config_file, 'r') as agent_config:
            config = json.load(agent_config)

        self._user_name = config['user_name']
        self._agent_name = config['agent_name']
        self._game_name = config['game_name']
        self._docker_image = config['docker_image']
        self._source_dir = config['source_dir']
        self._build_dir = config['build_dir']
        self._build_type = config['build_type']
        if self._build_type == 'cxx':
            self._cxx_source_name = config['cxx_source_name']
            self._cxx_executable_name = config.get('cxx_executable_name', 'a.out')
            self._cxx_compiler_name = config.get('cxx_compiler_name', 'g++')
            self._cxx_compiler_flag = config.get('cxx_compiler_flag', '-O3')
        else:
            raise NotImplementedError('unknown build_type = {}'.format(self.build_type))

    def build(self):
        log.debug('build')
        if self._docker_process is not None:
            log.debug('docker process is running already')
            return

        args = [
            'docker', 'run',
            '--rm',
            '-v', '{}:{}:ro'.format(self._agent_source_dir, '/source'),
            '-v', '{}:{}:rw'.format(self._agent_build_dir, '/build'),
            '-e', 'APGAME_AGENT_BUILD_TYPE={}'.format(self._build_type),
            '-e', 'APGAME_AGENT_SOURCE_DIR={}'.format("/source"),
            '-e', 'APGAME_AGENT_BUILD_DIR={}'.format("/build"),
            '-e', 'APGAME_CXX_SOURCE_NAME={}'.format(self._cxx_source_name),
            '-e', 'APGAME_CXX_EXECUTABLE_NAME={}'.format(self._cxx_executable_name),
            '-e', 'APGAME_CXX_COMPILER_NAME={}'.format(self._cxx_compiler_name),
            '-e', 'APGAME_CXX_COMPILER_FLAG={}'.format(self._cxx_compiler_flag),
            self._docker_image,
            '/bin/bash', '/usr/bin/agent-build.bash'
        ]
        log.debug('docker run: args = {}'.format(args))
        self._docker_process = subprocess.Popen(args)
        self._docker_process.wait()
        self._docker_process = None

    def kill(self):
        log.debug('kill')
        if self._docker_process is None:
            log.debug('docker process is not running')
            return
        self._docker_process.kill()
        self._docker_process = None

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        if self._docker_process is not None:
            self.kill()

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='apgame.agent')
    parser.add_argument('--agent-path', dest='agent_path', required=True)
    parser.add_argument('--log-level', dest='log_level', default='DEBUG')

    args = parser.parse_args()

    basicConfig()
    log.setLevel(getLevelName(args.log_level))

    with Agent(args.agent_path) as agent:
        agent.build()
        from IPython import embed
        embed()
