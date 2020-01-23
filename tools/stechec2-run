#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import argparse
import ctypes
import itertools
import os
import os.path
import shlex
import shutil
import subprocess
import sys
import tempfile
import time
import yaml
import signal

parser = argparse.ArgumentParser(
    description='Run stechec processes for a match',
    epilog='''Configuration file example:
    server: ./stechec2-server
    client: ./stechec2-client
    rules: libprologin2018.so
    map: ./simple.map
    verbose: 3
    clients:
      - ./champion.so
      - ./champion.so
    names:
      - Blue ones
      - Red ones
    spectators:
      - ./gui.so
    dump: dump.json

Report bugs to https://github.com/prologin/stechec2''',
    formatter_class=argparse.RawDescriptionHelpFormatter
)
parser.add_argument(
    '-v', '--version', action='store_true',
    help='Display version information'
)
parser.add_argument(
    '-q', '--quiet', action='store_true',
    help='Do not print messages'
)

# Selection of the processes to spawn
parser.add_argument(
    '-n', '--inhibit', metavar='P', type=int, action='append', default=[],
    help='Do not start process number P (0 = server, 1 = first client, ...'
    ' (champions first, spectators last)'
)
parser.add_argument(
    '-p', '--process', metavar='P', type=int, action='append', default=[],
    help='Start process number P only (same numbering scheme as for -n'
)

# Debugging options
parser.add_argument(
    '-d', '--debug', metavar='P', type=int, action='append', default=[],
    help='Debug process number P (same numbering scheme as for -n)'
)
parser.add_argument(
    '--debug-cmd', default='gdb --args',
    help='Command to use to debug. Default to "gdb --args".'
         ' For instance: "valgrind --leak-check=full"'
)
parser.add_argument(
    '-i', '--isolate', action='store_true',
    help='Isolate the clients with `isolate`'
)

parser.add_argument(
    'config-file', metavar='config-file',
    help='use F as a configuration file for the match'
)


def version():
    print('''stechec2-run.py

Copyright © 2013-2014, Prologin.
This is free software; see the source for copying conditions. There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.''')


LIBC = ctypes.CDLL('libc.so.6')
LIBC.strsignal.restype = ctypes.c_char_p


def signal_message(signal: int) -> str:
    return LIBC.strsignal(signal).decode()


def stechec2_run(args, options):
    popt = {'stderr': sys.stderr, 'stdout': sys.stdout}
    server_opt = []
    client_opt = []

    poll = []
    isolate_boxes = []

    def kill_handler(signal, frame):
        for _, p in poll:
            p.kill()
        for box in isolate_boxes:
            subprocess.call(['isolate', '--cg', '--box-id', str(box),
                             '--cleanup'])
        socket_dir.cleanup()
        sys.exit(1)

    signal.signal(signal.SIGINT, kill_handler)

    if args.inhibit and args.process:
        print('You cannot use both --inhibit and --process', file=sys.stderr)
        sys.exit(2)

    # Check options
    options_whitelist = ['server', 'client', 'rules', 'map', 'verbose',
                         'clients', 'names', 'spectators', 'dump', 'time',
                         'memory']
    for opt in options:
        if opt not in options_whitelist:
            sys.exit("Error in yaml config file, unknown option: " + opt)

    # Domain sockets
    socket_dir = tempfile.TemporaryDirectory(prefix='stechec2-')
    f_reqrep = socket_dir.name + '/reqrep'
    f_pubsub = socket_dir.name + '/pubsub'
    s_reqrep = 'ipc://' + f_reqrep
    s_pubsub = 'ipc://' + f_pubsub

    server_opt += [options.get('server', shutil.which('stechec2-server'))]
    client_opt += [options.get('client', shutil.which('stechec2-client'))]

    server_opt += ['--rep_addr', s_reqrep, '--pub_addr', s_pubsub]
    client_opt += ['--req_addr', s_reqrep, '--sub_addr', s_pubsub]

    clients = options.get('clients', [])
    client_names = options.get('names', [])
    spectators = options.get('spectators', [])

    if 'map' in options:
        options['map'] = os.path.realpath(options['map'])
    clients = list(map(os.path.realpath, clients))
    spectators = list(map(os.path.realpath, spectators))

    # Used by both client and server
    for i in ['rules', 'verbose', 'map', 'time', 'memory']:
        if i in options:
            server_opt += ['--' + i, str(options[i])]
            client_opt += ['--' + i, str(options[i])]

    # Used by server only

    clients_count = len(clients) + len(spectators)
    if clients_count == 0:
        sys.exit("Couldn't find any client nor spectator. "
                 "Did you forget some `spectators` or `clients` entries "
                 "in your yaml configuration?")

    server_opt += ['--nb_clients', str(clients_count)]
    for i in ['dump']:
        if i in options:
            server_opt += ['--' + i, str(options[i])]

    next_process = iter(itertools.count())

    def to_start(process_id):
        if args.process:
            return process_id in args.process
        elif args.inhibit:
            return process_id not in args.inhibit
        else:
            return True

    def start_proc(name, opts, popt):
        p = next(next_process)
        if p in args.debug:
            opts = shlex.split(args.debug_cmd) + opts
        cmd_line = ' '.join(opts)
        if to_start(p):
            if not args.quiet:
                print('>>> Starting {}[P={}]:'.format(name, p))
                print('    {}'.format(cmd_line))
            poll.append((name, subprocess.Popen(opts, **popt)))
        else:
            if not args.quiet:
                print('>>> Not starting {}[P={}]:'.format(name, p))
                print('    {}'.format(cmd_line))

    # Start the server
    start_proc('server', server_opt, popt)

    if args.isolate:
        os.chmod(socket_dir.name, 0o777)
        os.chmod(f_reqrep, 0o777)
        os.chmod(f_pubsub, 0o777)

    def run_client(client_id, client, name, is_spectator, isolate=False):
        opts = client_opt + ['--champion', client,
                             '--client_id', str(client_id),
                             '--name', name]
        if is_spectator:
            opts.append('--spectator')
        client_popt = dict(popt)
        client_popt['env'] = dict(os.environ)
        client_popt['env']['CHAMPION_PATH'] = os.path.dirname(
            os.path.abspath(client)
        )
        if isolate:
            prefix = os.path.realpath(os.path.dirname(client_opt[0]) + '/..')
            isolate_base = ['isolate', '--cg', '--box-id', str(client_id),
              '-d', os.path.realpath(os.path.dirname(socket_dir.name)) + ':rw',
              '-d', prefix,
              '-d', os.path.realpath('.'),
              '-d', os.path.dirname(client),
            ]
            if 'map' in options:
                isolate_base += ['-d', os.path.dirname(options['map'])]
            subprocess.call(isolate_base + ['--init'])
            opts = isolate_base + [
                    '--env', 'LD_LIBRARY_PATH=' +
                                os.environ.get('LD_LIBRARY_PATH', ''),
                    '--full-env',
                    '--processes',
                    '--run', '--',
                   ] + opts
            isolate_boxes.append(client_id)
        start_proc(name, opts, client_popt)

    # Start clients, then spectators
    client_id = 0
    for i, lib_so in enumerate(clients):
        client_id += 1
        lib_so = os.path.expanduser(lib_so)
        try:
            name = client_names[i]
        except IndexError:
            name = 'client-{}'.format(i + 1)
        run_client(client_id, lib_so, name, False, args.isolate)

    for i, lib_so in enumerate(spectators):
        client_id += 1
        lib_so = os.path.expanduser(lib_so)
        run_client(client_id, lib_so, 'spectator-{}'.format(i + 1), True)

    # Wait them all
    while poll:
        for i, (name, p) in enumerate(poll):
            r = p.poll()
            if r is not None:
                if r > 0:
                    print('>>> error: {} exited with code {}'.format(name, r))
                elif r < 0:
                    signal_code = -r
                    sig = signal.Signals(signal_code)
                    print('>>> error: {} killed by signal {}: {} ({})'
                        .format(name, signal_code, sig.name,
                                signal_message(signal_code)))
                poll.pop(i)
        time.sleep(0.1)

    for box in isolate_boxes:
        subprocess.call(['isolate', '--cg', '--box-id', str(box), '--cleanup'])
    socket_dir.cleanup()


if __name__ == '__main__':
    args = parser.parse_args()
    if args.version:
        version()
        sys.exit(0)
    else:
        config_file = vars(args)['config-file']
        try:
            c = yaml.safe_load(open(config_file))
        except yaml.YAMLError as e:
            print('Parse error in {}: {}'.format(config_file, e))
            sys.exit(1)
        stechec2_run(args, c)
