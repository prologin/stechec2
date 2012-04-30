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

import gevent.queue

class CompilationTask(object):
    def __init__(self, config, user, champ_id):
        self.contest = config['master']['contest']
        self.secret = config['master']['shared_secret']
        self.user = user
        self.champ_id = champ_id

    @property
    def slots_taken(self):
        return 1

    def execute(self, master, worker):
        worker.rpc.compile_champion(
            self.secret, self.contest, self.user, self.champ_id
        )

    def __repr__(self):
        return "<Compilation: %s/%d>" % (self.user, self.champ_id)

class PlayerTask(object):
    def __init__(self, config, mid, hostname, req_port, sub_port, cid, mpid, user, opts):
        self.contest = config['master']['contest']
        self.secret = config['master']['shared_secret']
        self.mid = mid
        self.hostname = hostname
        self.req_port = req_port
        self.sub_port = sub_port
        self.cid = cid
        self.mpid = mpid
        self.user = user
        self.opts = opts

    @property
    def slots_taken(self):
        return 2 # It's usually fairly intensive, take 2 slots

    def execute(self, master, worker):
        worker.rpc.run_client(
            self.secret, self.contest, self.mid, self.hostname, self.req_port,
            self.sub_port, self.user, self.cid, self.mpid, self.opts
        )

class MatchTask(object):
    def __init__(self, config, mid, players, opts):
        self.config = config
        self.contest = config['master']['contest']
        self.secret = config['master']['shared_secret']
        self.mid = mid
        self.players = players
        self.opts = opts
        self.server_port = gevent.queue.Queue()
        self.player_tasks = set()

    @property
    def slots_taken(self):
        return 1 # Only the server is launched by this task

    def execute(self, master, worker):
        master.matches[self.mid] = self
        worker.rpc.run_server(
            self.secret, self.contest, self.mid, self.opts
        )
        req_port = self.server_port.get()
        sub_port = self.server_port.get()
        for (cid, mpid, user) in self.players:
            # on error, prevent launching several times the players
            if mpid in self.player_tasks:
                continue
            self.player_tasks.add(mpid)

            t = PlayerTask(self.config, self.mid, worker.hostname, req_port,
                sub_port, cid, mpid, user, self.opts)
            master.worker_tasks.append(t)
        master.to_dispatch.set()
        del master.matches[self.mid]
