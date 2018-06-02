=====
Usage
=====

Build procedure
---------------

Stechec2's build system is based on `Waf <https://code.google.com/p/waf/>`_.
The build procedure is composed of the two traditional steps: configure and
build itself.

The configure step is the one in which you specify where you want to install
Stechec2, with what games, how, etc. At the root of the source directory, run:

.. code-block:: bash

  ./waf.py configure --prefix=/home/user/stechec-install --with-games=tictactoe

This will configure the build system to build Stechec2 with the ``tictactoe``
game and to install it in ``/home/user/stechec-install`` (i.e. put executables
in the ``bin`` subdirectory, libraries in ``lib``, etc.). Note that this
also creates the ``build`` directory, where all compilation artifacts (object
files, test programs) will go.

Now, build Stechec2 and the corresponding games:

.. code-block:: bash

  ./waf.py build

And if everything went well, install it!

.. code-block:: bash

  ./waf.py install


Create your AI
--------------

Once the game is installed, you need to create an AI for it. To do so, generate
the player environment (with different folders for each supported languages):

.. code-block:: bash

  stechec2-generator player tictactoe player_env

You might need to install `languages dependencies
<https://github.com/prologin/stechec2/#install-languages-dependencies>`_.

Go to the folder corresponding to the programming language you want to code in,
and start editing the file :code:`prologin`. A Makefile is provided to create a
tarball containing all your source files (don't forget to update the Makefile if
you add new files):

.. code-block:: bash

  make tar

Launch a match
--------------

Stechec2 is based on a server-client architecture, hence you need to launch
:code:`stechec2-server` and one :code:`stechec2-client` per player. This can be
easily done with a wrapper called :code:`stechec2-run` which runs everything
needed. You only have to write a small YAML configuration file.

An example might be :code:`config.yml`::

  rules: /usr/lib/libtictactoe.so
  map: ./simple.map
  verbose: 3
  clients:
    - ./champion.so
    - /path/to/other/champion.so
  names:
    - Player 1
    - Player 2

A match can now be simply started with:

.. code-block:: bash

  stechec2-run config.yml

Add spectators
--------------

To watch a game, you can add a spectator, which is a player that don't take part
in the game.

First, compile your spectator:

.. code-block:: bash

  cd /path/to/prologin2014/gui
  make

Then, add those lines to your :code:`config.yml`::

  spectators:
    - /path/to/prologin2014/gui/gui.so

Debugging
---------

To use gdb with stechec2 you can add the ``--debug`` option (or simply ``-d``)
followed by the client id you want to debug:

.. code-block:: bash

    stechec2-run -d 1 config.yml

You can find other useful options by running:

.. code-block:: bash

    stechec2-run -h
