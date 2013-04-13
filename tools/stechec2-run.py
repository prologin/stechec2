#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import yaml
import sys
import subprocess


def version():
    print('''stechec2-run.py

Copyright © 2012, Prologin.
This is free software; see the source for copying conditions. There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Written by Antoine Pietri.''')


def usage():
    print('''Usage: {0} [-h|-v] <config-file.yml>

Example:
   {0} config.yml       to run a match, using configuration file config.yml

Configuration file example:
    rules: libprologin2013.so
    map: ./simple.map
    verbose: 3
    clients:
      - ./champion.so
      - ./champion.so
    spectators:
      - ./dumper.so
      - ./gui.so

Report bugs to <serveur@prologin.org>'''.format(sys.argv[0]))


def stechec2_run(options):
    poll = []

    popt = {'stderr': sys.stdin, 'stdout': sys.stdout}
    server_opt = ['stechec2-server']
    client_opt = ['stechec2-client']

    # Used by both client and server
    for i in ['rules', 'verbose', 'map']:
        if i in options:
            server_opt += ['--' + i, str(options[i])]
            client_opt += ['--' + i, str(options[i])]

    # Used by server only
    clients = options.get('clients', [])
    spectators = options.get('spectators', [])
    server_opt += ['--nb_clients', str(len(clients) + len(spectators))]

    # Start the server
    poll.append(subprocess.Popen(server_opt, **popt))

    def run_client(client, name, is_spectator):
        opts = client_opt + ['--champion', client, '--name', name]
        if is_spectator:
            opts.append('--spectator')
        poll.append(subprocess.Popen(opts,**popt))

    # Start clients, then spectators
    for i, lib_so in enumerate(clients):
        run_client(lib_so, 'client-{}'.format(i + 1), False)

    for i, lib_so in enumerate(spectators):
        run_client(lib_so, 'spectator-{}'.format(i + 1), True)

    # Wait them all
    for p in poll:
        p.wait()


if __name__ == '__main__':
    if len(sys.argv) < 2 or sys.argv[1] in ['-h', '--help']:
        usage()
        sys.exit(1)
    elif sys.argv[1] in ['-v', '--version']:
        version()
        sys.exit(1)
    else:
        try:
            c = yaml.load(open(sys.argv[1]))
        except yaml.YAMLError as e:
            print('Parse error in {}: {}'.format(sys.argv[1], e))
            sys.exit(1)
        stechec2_run(c)
