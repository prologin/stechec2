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
    for i in ['nb_clients']:
        if i in options:
            server_opt += ['--' + i, str(options[i])]

    poll.append(subprocess.Popen(server_opt, **popt))
    for i, client in enumerate(options['clients']):
        poll.append(subprocess.Popen(client_opt + 
            ['--champion', client, '--name', str(i + 1)], **popt))

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
