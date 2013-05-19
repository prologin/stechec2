Stechec2
========

What is Stechec2
----------------

Stechec2 is a client-server turn-based strategy game match maker used during the
French national computing contest. It is a complete rewrite of Stechec to
achieve a simpler architecture.

Copying
-------

Free use of this software is granted under the terms of the GNU General Public
License version 2 (GPLv2). For details see the files `COPYING` included with
the Stechec2 distribution.

Requirements
------------

* gcc >= 4.7
* python2
* zeromq
* zeromq C++ wrapper
* google-gflags
* googletest
* ruby (only for the generators)
* python-yaml

Installation
------------

Let's assume you want to install stechec2 with the prologin2013 game, and then
use it as a player.

Clone the stechec2 repository::

  git clone https://bitbucket.org/prologin/stechec2.git
  cd stechec2

Then put every game you want to install in ``games/``::

  git clone https://bitbucket.org/prologin/prologin2012.git games/
  git clone https://bitbucket.org/prologin/prologin2013.git games/

A simple test game, ``tictactoe``, is already installed in ``games/``.

You can now configure the project using waf::

  python2 waf.py configure --with-games=prologin2013,tictactoe --prefix=/usr

Then build and install it::

  python2 waf.py build install

Generate the player environment
---------------------------------

To generate the player environment (different folders for each supported
languages), you can use the ``generator`` script installed by stechec2::

  generator player /usr/share/stechec2/prologin2013/prologin2013.yml home_env/

Create your AI
--------------

You should now be able to choose your favorite language folder and begin to
code::

  cd home_env/python/
  $EDITOR prologin.py
  make

To create a tarball containing all your source files (you can add some by
editing the Makefile), do::

  make tar

Launch a match
--------------

Launch a stechec2 server and as many stechec2 client as there are players is
very painful and can be avoided easily. There is a wrapper called
``stechec2-run`` which runs everything you need in separate child processes,
and only needs a tiny YAML configuration file to work.

A simple ``config.yml`` could be::

  rules: /usr/lib/libprologin2013.so
  map: ./simple.map
  verbose: 3
  clients:
    - ./champion.so
    - /path/to/other/champion.so
  names:
    - Player 1
    - Player 2

Then you can just launch the match easily::

  stechec2-run config.yml

Add spectators
--------------

Spectators are players that don't take part of the game, but can watch its
different states during the match (to display it or to log it, for instance).

Make sure to compile the spectators first::

  cd /path/to/prologin2013/gui
  make
  cd /path/to/prologin2013/dumper
  make

Then you just have to add those lines to the ``config.yml``::

  spectators:
   - /path/to/prologin2013/gui/gui.so
   - /path/to/prologin2013/dumper/dumper.so

