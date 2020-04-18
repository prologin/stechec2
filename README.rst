Stechec2
========

.. image:: https://travis-ci.org/prologin/stechec2.svg?branch=master
    :target: https://travis-ci.org/prologin/stechec2

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
* python
* zeromq
* zeromq C++ wrapper
* google-gflags
* googletest
* gcovr (for code coverage reports)
* python-yaml
* python-jinja2

Arch Linux::

  pacman --needed -S gcc zeromq gtest python-yaml python-jinja gflags gcovr

Debian/Ubuntu::

  apt-get install build-essential libzmq3-dev libzmqpp-dev python3-yaml \
      python3-jinja2 libgtest-dev libgflags-dev gcovr


Installation
------------

Let's assume you want to install stechec2 with all the Prologin games, and then
use it as a player.

Clone the stechec2 repository::

  git clone https://github.com/prologin/stechec2
  cd stechec2

Then put every game you want to install in ``games/``::

  for game in prologin{2012..2016}; do
    git clone https://github.com/prologin/${game}.git games/$game
  done

A simple test game, ``tictactoe``, is already installed in ``games/``.

You can then configure the project using waf, while specifying the games you
want to use::

  ./waf.py configure --with-games=tictactoe,prologin2016 --prefix=/usr

Then build and install it::

  ./waf.py build install

**Archlinux**: A PKGBUILD is available in ``pkg/stechec2``:
run ``makepkg && pacman -U stechec2-*.pkg.tar.xz``.

Generate the player environment
---------------------------------

To generate the player environment (different folders for each supported
languages), you can use the ``stechec2-generator`` script installed by
stechec2::

  stechec2-generator player tictactoe player_env

Install languages dependencies
------------------------------

Requirements:

- php (warning: you may need to add ``open_basedir=/`` to your
  ``/etc/php/php.ini``)
- php-embed
- ocaml
- mono
- jdk-java
- ghc
- rustc

Archlinux::

  pacman --needed -S php php-embed ocaml mono jdk11-openjdk ghc rust

Debian/Ubuntu::

  apt-get install php-cli php-dev libphp-embed ocaml mono-devel ghc \
                  openjdk-11-jdk rustc

Create your AI
--------------

You should now be able to choose your favorite language folder and begin to
code::

  cd player_env/python/
  $EDITOR prologin.py
  make

To create a tarball containing all your source files (you can add some by
editing the Makefile), do::

  make tar

Launch a match
--------------

To launch a match, you need to launch a ``stechec2-server`` and one
``stechec2-client`` per player. There is a wrapper called ``stechec2-run``
which runs everything you need in separate child processes, and only needs a
tiny YAML configuration file to work.

A simple ``config.yml`` could be::

  rules: /usr/lib/libtictactoe.so
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

Make sure to compile your spectator first::

  cd /path/to/prologin2014/gui
  make

Then you just have to add those lines to the ``config.yml``::

  spectators:
   - /path/to/prologin2014/gui/gui.so
