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
import task
import time
import xmlrpclib

class Worker(object):
    def __init__(self, hostname, port, slots, max_slots):
        self.hostname = hostname
        self.port = port
        self.slots = slots
        self.max_slots = max_slots
        self.tasks = []
        self.keep_alive()

    @property
    def usage(self):
        return 1.0 - (float(self.slots) / self.max_slots)

    def update(self, slots, max_slots):
        self.slots = slots
        self.max_slots = max_slots
        self.keep_alive()

    def keep_alive(self):
        self.last_heartbeat = time.time()

    def is_alive(self, timeout):
        return (time.time() - self.last_heartbeat) < timeout

    def can_add_task(self, task):
        return self.slots >= task.slots_taken

    def add_task(self, master, task):
        self.slots -= task.slots_taken
        greenlet = gevent.spawn(task.execute, master, self)
        self.tasks.append((task, greenlet))

    def kill_tasks(self):
        for (t, g) in self.tasks:
            g.kill()

    def remove_compilation_task(self, champ_id):
        new = []
        for (t, g) in self.tasks:
            if isinstance(t, task.CompilationTask):
                if t.champ_id == champ_id:
                    continue
            new.append((t, g))
        self.tasks = new

    def remove_match_task(self, mid):
        new = []
        for (t, g) in self.tasks:
            if isinstance(t, task.MatchTask):
                if t.mid == mid:
                    continue
            new.append((t, g))
        self.tasks = new

    def remove_player_task(self, mpid):
        new = []
        for (t, g) in self.tasks:
            if isinstance(t, task.PlayerTask):
                if t.mpid == mpid:
                    continue
            new.append((t, g))
        self.tasks = new

    @property
    def rpc(self):
        url = "http://%s:%d/" % (self.hostname, self.port)
        return xmlrpclib.ServerProxy(url)

    def __repr__(self):
        return '<Worker: %s:%d>' % (self.hostname, self.port)
