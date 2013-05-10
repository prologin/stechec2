# -*- encoding: utf-8 -*-
# This file is part of Stechec.
#
# Copyright (c) 2011 Pierre Bourdon <pierre.bourdon@prologin.org>
# Copyright (c) 2011 Association Prologin <info@prologin.org>
#
# Stechec is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Stechec is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Stechec.  If not, see <http://www.gnu.org/licenses/>.

from subprocess import Popen, PIPE, STDOUT

import errno
import fcntl
import gevent
import gevent.socket
import gzip
import paths
import os
import os.path
import sys
import tempfile

def communicate(cmdline, new_env=None, data=''):
    """
    Asynchronously communicate with an external process, sending data on its
    stdin and receiving data from its stdout and stderr streams.

    Returns (retcode, stdout).
    """
    p = Popen(cmdline, stdin=PIPE, stdout=PIPE, stderr=STDOUT, env=new_env)
    fcntl.fcntl(p.stdin, fcntl.F_SETFL, os.O_NONBLOCK)
    fcntl.fcntl(p.stdout, fcntl.F_SETFL, os.O_NONBLOCK)

    # Write stdin data
    total_bytes = len(data)
    written_bytes = 0
    while written_bytes < total_bytes:
        try:
            written_bytes += os.write(p.stdin.fileno(), data[written_bytes:])
        except IOError, ex:
            if ex[0] != errno.EAGAIN:
                raise
            sys.exc_clear()
        gevent.socket.wait_write(p.stdin.fileno())
    p.stdin.close()

    # Read stdout data
    chunks = []
    read_size = 0
    while True:
        try:
            chunk = p.stdout.read(4096)
            read_size += len(chunk)
            if not chunk:
                break

            if read_size < 2**32:
                chunks.append(chunk)
        except IOError, ex:
            if ex[0] != errno.EAGAIN:
                raise
            sys.exc_clear()
        gevent.socket.wait_read(p.stdout.fileno())
    p.stdout.close()

    if read_size >= 2**18:
        chunks.append("\n\nLog truncated to stay below 256K\n")

    # Wait for process completion
    """while p.poll() is None:
        gevent.sleep(0.05)"""
    gevent.sleep(0.1)
    try:
        p.kill()
    except OSError:
        pass
    # Return data
    return (0, ''.join(chunks))

def champion_path(config, contest, user, champ_id):
    return os.path.join(config['contest']['directory'], contest, 'champions',
                        user, str(champ_id))

def match_path(config, contest, match_id):
    match_id_high = "%03d" % (match_id / 1000)
    match_id_low = "%03d" % (match_id % 1000)
    return os.path.join(config['contest']['directory'], contest, "matches",
                        match_id_high, match_id_low)

def compile_champion(config, contest, user, champ_id):
    """
    Compiles the champion at $dir_path/champion.tgz to $dir_path/champion.so.

    Returns a tuple (ok, output), with ok = True/False and output being the
    output of the compilation script.
    """
    dir_path = champion_path(config, contest, user, champ_id)
    cmd = [paths.compile_script, config['contest']['directory'], dir_path]
    retcode, stdout = communicate(cmd)
    return retcode == 0 and os.path.exists(os.path.join(dir_path, 'champion.so'))

def spawn_server(cmd, path, match_id, callback):
    retcode, stdout = communicate(cmd)

    log_path = os.path.join(path, "server.log")
    open(log_path, "w").write(stdout)
    callback(retcode, stdout, match_id)

def spawn_dumper(cmd, path):
    dump_path = tempfile.mktemp()
    def set_path():
        os.environ['DUMP_PATH'] = dump_path
    p = Popen(cmd, stdin=PIPE, stdout=PIPE, stderr=STDOUT, preexec_fn=set_path)
    fcntl.fcntl(p.stdin, fcntl.F_SETFL, os.O_NONBLOCK)
    fcntl.fcntl(p.stdout, fcntl.F_SETFL, os.O_NONBLOCK)
    p.stdin.close()
    # Read stdout data
    while True:
        try:
            chunk = p.stdout.read(4096)
            print(chunk)
            if not chunk:
                break
        except IOError, ex:
            if ex[0] != errno.EAGAIN:
                raise
            sys.exc_clear()
        gevent.socket.wait_read(p.stdout.fileno())
    p.stdout.close()

    # Wait for process completion
    while p.poll() is None:
        gevent.sleep(0.05)

    # Compress the dump and place it to its final destination
    final_dump = os.path.join(path, "dump.json.gz")
    final_fp = gzip.open(final_dump, "w")
    fp = open(dump_path, "r")
    for line in fp:
        final_fp.write(line)
    fp.close()
    final_fp.close()
    os.unlink(dump_path)

def run_server(config, server_done, rep_port, pub_port, contest, match_id, opts):
    """
    Runs the Stechec server and wait for client connections.
    """
    path = match_path(config, contest, match_id)
    try:
        os.makedirs(path)
    except OSError:
        pass

    opts_dict = {}
    for line in opts.split('\n'):
        if '=' not in line:
            continue
        name, value = line.split('=', 1)
        opts_dict[name.strip()] = value.strip()

    dumper = config['contest']['dumper']
    cmd = [paths.stechec_server,
                "--map", opts_dict['map'],
                "--rules", paths.libdir + "/lib" + contest + ".so",
                "--rep_addr", "tcp://*:%d" % rep_port,
                "--pub_addr", "tcp://*:%d" % pub_port,
                "--nb_clients", "3",
                "--verbose", "1"]
    gevent.spawn(spawn_server, cmd, path, match_id, server_done)
    gevent.sleep(0.5) # let it start
    # XXX: for some reasons clients now have to be run after the server
    # otherwise they misbehave. Temporary fix.
    nb_spectator = 1 if dumper else 0
    if nb_spectator:
        cmd = [paths.stechec_client, 
               "--map", opts_dict['map'],
               "--name", "dumper",
               "--rules", paths.libdir + "/lib" + contest + ".so",
               "--champion", dumper,
               "--req_addr", "tcp://localhost:%d" % rep_port,
               "--sub_addr", "tcp://localhost:%d" % pub_port,
               "--memory", "250000",
               "--time", "3000",
               "--spectator",
               "--verbose", "1"]
        gevent.spawn(spawn_dumper, cmd, path)

def spawn_client(cmd, env, path, match_id, champ_id, tid, callback):
    retcode, stdout = communicate(cmd, env)
    log_path = os.path.join(path, "log-champ-%d-%d.log" % (tid, champ_id))
    open(log_path, "w").write(stdout)
    callback(retcode, stdout, match_id, champ_id, tid)

def run_client(config, ip, req_port, sub_port, contest, match_id, user, champ_id, tid, opts, cb):
    dir_path = champion_path(config, contest, user, champ_id)
    mp = match_path(config, contest, match_id)

    opts_dict = {}
    for line in opts.split('\n'):
        if '=' not in line:
            continue
        name, value = line.split('=', 1)
        opts_dict[name.strip()] = value.strip()

    env = os.environ.copy()
    env['CHAMPION_PATH'] = dir_path + '/'

    cmd = [paths.stechec_client,
                "--map", opts_dict['map'],
                "--name", str(tid),
                "--rules", paths.libdir + "/lib" + contest + ".so",
                "--champion", dir_path + "/champion.so",
                "--req_addr", "tcp://{ip}:{port}".format(ip=ip, port=req_port),
                "--sub_addr", "tcp://{ip}:{port}".format(ip=ip, port=sub_port),
                "--memory", "250000",
                "--time", "1500"
          ]
    gevent.spawn(spawn_client, cmd, env, mp, match_id, champ_id, tid, cb)
