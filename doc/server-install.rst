=================================================
Installing the server side components of Stechec2
=================================================

The server side components of Stechec2 are required if you want to host a
competition using the framework provided by Stechec2. These components include:

* The contest website which is the user-facing interface of Stechec2. It
  allows players to upload their code, launch matches, show the tournaments
  results, etc.
* The worker node script, which can be launched on any number of machines you
  want. Each node has a given number of slots that represents the number of
  tasks that can be executed on this node simultaneously. A task is for example
  a champion compilation or a match launch.
* The master node script, which interfaces the website and the worker nodes. It
  distributes the tasks on the worker nodes cluster and updates the server
  database with the results from the tasks.

Prerequisites
-------------

* PostgreSQL 8.3 or newer.
* A filesystem shared by the worker nodes and the website. Stechec2 has only
  been tested using NFS4 when this doc was written but any POSIX compliant
  filesystem should work fine (GFS, GlusterFS, Ceph, AFS, ...).
* Python 2.6 or newer.
* Some Python libraries: `Django`_ (v1.4), `gevent`_, `yaml`_.

.. _Django: http://pypi.python.org/pypi/Django
.. _gevent: http://pypi.python.org/pypi/gevent
.. _yaml: http://pypi.python.org/pypi/PyYAML

Install the applications
------------------------

The server components are installed with the client components of Stechec2. To
install the server componenents on your machine, follow the client installation
documentation.

Website deployment
------------------

The website has to be installed first because it also creates the database
schema used by the master node. In this documentation we will use `gunicorn`_
to host the WSGI application, with `nginx`_ put in front to host the static
files.

.. _gunicorn: http://gunicorn.org/
.. _nginx: http://nginx.org/

First of all, copy the sample configuration file from
``${PREFIX}/share/stechec2/www/concours/website.yml`` to
``/etc/stechec/website.yml``. In there, change the configuration settings to
match your server configuration:

* The database informations must point to the database that will be used by the
  website and the master node.
* The master node URL does not have to be correct the first time around, you
  can change it later after having installed the master node.
* The contest informations must match the game for which you want to host a
  contest. The ``directory`` key must point to the shared directory mentioned
  in the prerequisites.
* Generate a random secret key and use it to replace the ``CHANGEME`` in the
  configuration file.

Then go to ``${PREFIX}/share/stechec2/www/concours`` and use the ``manage.py``
script to generate the initial database schema and create the admin user
account::

    $ python manage.py syncdb

You can then proceed to host the Django application in this directory like a
normal Django website. Using ``gunicorn`` and ``nginx``, that means running
``gunicorn_django`` like this::

    $ gunicorn_django -D --error-logfile=error.log

And configuring an ``nginx`` virtual host like this one::

    server {
        listen 80;
        server_name localhost;
        location / {
            proxy_pass http://127.0.0.1:8000/;
        }
        location /static {
            root ${PREFIX}/share/stechec2/www/concours/stechec;
        }
    }

Master node deployment
----------------------

The master node is very easy to deploy: copy its sample configuration file from
``${PREFIX}/share/stechec2/master-node/master-node.yml`` to ``/etc/stechec``
and change the database informations and the contest game name. After that,
launch ``master-node`` and the master node should be running properly and
logging informations to syslog.

Don't forget to accept connections to the master node in your firewall, and
change the master node URL in the website configuration if you haven't already
done so.
