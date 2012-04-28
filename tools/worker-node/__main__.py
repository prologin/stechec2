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

import gevent
import gevent.monkey
import gevent.socket

# We do not use .patch_all() because .patch_os() must NOT be called.
#
# It overrides os.fork() which is used in subprocess.Popen and causes a
# strange bug in libevent when the hub is reinitialized (infinite loop on ^C).
gevent.monkey.patch_select()
gevent.monkey.patch_socket()
gevent.monkey.patch_ssl()
gevent.monkey.patch_time()
gevent.monkey.patch_thread()

from SimpleXMLRPCServer import SimpleXMLRPCServer

import logging
import logging.handlers
import os.path
import operations
import optparse
import paths
import re
import socket
import xmlrpclib
import yaml

class WorkerNode(object):
    def __init__(self, config):
        self.config = config
        self.interval = config['master']['heartbeat_secs']
        self.hostname = socket.gethostname()
        self.port = config['worker']['port']
        self.slots = self.max_slots = config['worker']['available_slots']
        self.secret = config['master']['shared_secret']
        self.min_srv_port = config['worker']['port_range_start']
        self.max_srv_port = config['worker']['port_range_end']
        self.srv_port = self.min_srv_port
        self.matches = {}

        self.heartbeat_greenlet = gevent.spawn(self.send_heartbeat)

    def get_worker_infos(self):
        return (self.hostname, self.port, self.slots, self.max_slots)

    @property
    def master(self):
        config = self.config
        host, port = (config['master']['host'], config['master']['port'])
        url = "http://%s:%d/" % (host, port)
        return xmlrpclib.ServerProxy(url)

    @property
    def available_server_port(self):
        """
        Be optimistic and hope that:
        - nobody will use the ports in the port range but us
        - there will never be more servers than ports in the range
        """
        port = self.srv_port
        self.srv_port += 1
        if self.srv_port > self.max_srv_port:
            self.srv_port = self.min_srv_port
        return port

    def send_heartbeat(self):
        logging.info('sending heartbeat to the server, %d/%d slots' % (
                         self.slots, self.max_slots
        ))
        first_heartbeat = True
        while True:
            try:
                self.master.heartbeat(self.secret, self.get_worker_infos(),
                                      first_heartbeat)
                first_heartbeat = False
            except gevent.socket.error:
                msg = 'master down, retrying heartbeat in %ds' % self.interval
                logging.warn(msg)
            gevent.sleep(self.interval)

    def start_work(self, work, slots, *args, **kwargs):
        if self.slots < slots:
            logging.warn('not enough slots to start the required job')
            return False, self.secret, self.get_worker_infos()

        logging.debug('starting a job for %d slots' % slots)
        self.slots -= slots

        def real_work():
            job_done = True
            try:
                job_done, func, args_li = work(*args, **kwargs)
            finally:
                if job_done:
                    self.slots += slots
            func(self.secret, self.get_worker_infos(), *args_li)

        gevent.spawn(real_work)
        return True, self.secret, self.get_worker_infos()

    def compile_champion(self, contest, user, champ_id):
        ret = operations.compile_champion(self.config, contest, user, champ_id)
        return True, self.master.compilation_result, (champ_id, ret)

    def run_server(self, contest, match_id, opts=''):
        logging.info('starting server for match %d' % match_id)
        rep_port = self.available_server_port
        pub_port = self.available_server_port
        operations.run_server(self.config, worker.server_done, rep_port,
                            pub_port, contest, match_id, opts)
        return False, self.master.match_ready, (match_id, rep_port, pub_port)

    def server_done(self, retcode, stdout, match_id):
        self.slots += 1

        logging.info('match %d done' % match_id)

        lines = stdout.split('\n')
        result = []
        score_re = re.compile(r'^(\d+) (\d+) (-?\d+) \(.*\)$')
        for line in lines:
            m = score_re.match(line)
            if m is None:
                continue
            pid, score, stat = m.groups()
            result.append((int(pid), int(score)))

        try:
            self.master.match_done(self.secret, self.get_worker_infos(),
                                   match_id, result)
        except socket.error:
            pass

    def run_client(self, contest, match_id, ip, req_port, sub_port, user, champ_id, pl_id, opts):
        logging.info('running champion %d from %s for match %d' % (
                         champ_id, user, match_id
        ))
        operations.run_client(self.config, ip, req_port, sub_port, contest, match_id, user,
                              champ_id, pl_id, opts, self.client_done)
        return False, self.master.client_ready, (match_id, pl_id)

    def client_done(self, retcode, stdout, match_id, champ_id, pl_id):
        self.slots += 2
        logging.info('champion %d for match %d done' % (champ_id, match_id))
        try:
            self.master.client_done(self.secret, self.get_worker_infos(),
                                    match_id, pl_id, retcode)
        except socket.error:
            pass

class WorkerNodeProxy(object):
    """
    Proxies XMLRPC requests to the WorkerNode.
    """

    def __init__(self, node):
        self.node = node

    def compile_champion(self, secret, *args, **kwargs):
        logging.debug('received a compile_champion request')
        return self.node.start_work(
            self.node.compile_champion, 1, *args, **kwargs
        )

    def run_server(self, secret, *args, **kwargs):
        logging.debug('received a run_server request')
        return self.node.start_work(
            self.node.run_server, 1, *args, **kwargs
        )

    def run_client(self, secret, *args, **kwargs):
        logging.debug('received a run_client request')
        return self.node.start_work(
            self.node.run_client, 2, *args, **kwargs
        )

def read_config(filename):
    return yaml.load(open(filename))

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('-c', '--config', dest='config_file',
                      default=paths.config_file,
                      help='The configuration file.')
    parser.add_option('-l', '--local-logging', action='store_true',
                      dest='local_logging', default=False,
                      help='Activate logging to stdout.')
    parser.add_option('-v', '--verbose', action='store_true',
                      dest='verbose', default=False,
                      help='Verbose mode.')
    options, args = parser.parse_args()

    loggers = []
    loggers.append(logging.handlers.SysLogHandler('/dev/log'))
    if options.local_logging:
        loggers.append(logging.StreamHandler())
    for logger in loggers:
        logger.setFormatter(logging.Formatter(
            'worker-node: [%(levelname)s] %(message)s'
        ))
        logging.getLogger('').addHandler(logger)
    logging.getLogger('').setLevel(
        logging.DEBUG if options.verbose else logging.INFO
    )

    config = read_config(options.config_file)
    s = SimpleXMLRPCServer(('0.0.0.0', config['worker']['port']),
                           logRequests=False)
    s.register_introspection_functions()

    worker = WorkerNode(config)
    s.register_instance(WorkerNodeProxy(worker))

    try:
        s.serve_forever()
    except KeyboardInterrupt:
        pass
