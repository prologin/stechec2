=====
Usage
=====

Build and procedure
-------------------

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


Run procedure
-------------

TODO
