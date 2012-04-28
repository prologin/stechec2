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

import gevent.socket
import psycopg2
import psycopg2.extensions

"""
Implements a wait function for Psycopg in order to wait for events while being
nice with gevent (not blocking, etc.).

See http://initd.org/psycopg/docs/advanced.html#asynchronous-support
"""

def init_psycopg_gevent():
    def wait(conn, timeout=None):
        while True:
            state = conn.poll()
            if state == psycopg2.extensions.POLL_OK:
                break
            elif state == psycopg2.extensions.POLL_READ:
                gevent.socket.wait_read(conn.fileno(), timeout=timeout)
            elif state == psycopg2.extensions.POLL_WRITE:
                gevent.socket.wait_write(conn.fileno(), timeout=timeout)
            else:
                raise psycopg2.OperationalError("invalid poll state")

    psycopg2.extensions.set_wait_callback(wait)
