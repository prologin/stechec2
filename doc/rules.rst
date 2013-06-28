==================================================
How to implement the rules of a game in stechec2 ?
==================================================

Introduction
============

So you want to organize a big contest between IAs and you don't know where to
start ? This tutorial is for you ! We're going to implement a basic [Connect
Four](http://en.wikipedia.org/wiki/Connect_Four) game in this tutorial, step by
step, to understand the mechanics of stechec2's rules writing.

If you're stuck in some part, you can help yourself with the tictactoe game
that has already been implemented in ``games/tictactoe``.


Think about The Game
====================

We must define first *exactly* the rules of our game: how it works, who plays
first, how many players a play can handle, who wins…
The rules of the Connect Four are very simple : two players, a turn by turn
game, only one action (drop a jeton somewhere).

We can begin to describe the rules in the .yml::

  $ mkdir connect4
  $ cd connect4
  $ vim connect4.yml

Write the YAML
==============

First, we must write some config boilerplate at the top of the file::

  conf:
      package_name: connect4     # The name of the game
      player_lib: champion       # The .so containing the code of the player
      player_filename: main      # The main file where the user should code
      server_rule_lib: connect4  # The .so containing the rules (server)
      client_rule_lib: connect4  # The .so containing the rules (client)
  constant:
    # Place your constants here
  enum:
    # Place your enums here
  struct:
    # Place your structs here
  function:
    # Place the functions of the API here
  user_function:
    # Place the functions that the player sould implement here

The type system
---------------

Stechec2 uses its generators to implements some basic types in every supported
languages. When a field requires a type in the yaml, you can use the built-in
types (``int``, ``bool``  and ``string``), the structs you defined, the enums
you defined and ``_ array`` (where _ is a type itself). You can even use arrays
of arrays (``int array array array``, for instance, will create a 3D matrix of
ints).

The player functions
--------------------

Depending of how the game works, the player should implement some functions
in which he calls the actions of the API. Usually, this is done by:

* A function ``init_game``, called at the start of the game, in which the
  player can create and initialize his objects
* A function ``play_turn``, called at each turn, in which he can play (call the
  API functions).
* A function ``end_game``, called at the end of the game, in which he can
  delete and free his objects.

We're going to use these functions for our connect4, which does not require
more, but the player can have more functions to implement in a phase based
game: ``play_move_phase`` and ``play_attack_phase``, for instance.

In the yaml, a function is described like this:

* ``fct_name``: the name of the function.
* ``fct_summary``: a short documentation of the function.
* ``fct_ret_type``: the return type of the function (bool, void, int array…).
* ``fct_arg``: the list of arguments that the function takes. Each item is a
  list containing:

  * The name of the argument.
  * The type of the argument.
  * A short description of its use.

So, let's write our user functions::

  user_function:
    -
      fct_name: init_game
      fct_summary: Function called at the start of the game
      fct_ret_type: void
      fct_arg: []
    -
      fct_name: play_turn
      fct_summary: Function called at each turn
      fct_ret_type: void
      fct_arg: []
    -
      fct_name: end_game
      fct_summary: Function called at the end of the game
      fct_ret_type: void
      fct_arg: []

The constants
-------------

A constant is described by three fields:

* ``cst_name``: the name of the constant.
* ``cst_val``: the value of the constant.
* ``cst_comment``: a short description of the constant.

The only constants we'll use in our game are the constants describing the size
of the board, and the limit of players::

  constant:
    -
      cst_name: NB_COLS
      cst_val: 8
      cst_comment: number of columns in the board
    -
      cst_name: NB_ROWS
      cst_val: 6
      cst_comment: number of rows in the board
    -
      cst_name: NB_PLAYERS
      cst_val: 2
      cst_comment: number of players during the game


The enumerations
----------------

An enumeration is described by three fields:

* ``enum_name``: the name of the enum.
* ``enum_summary``: a short documentation of the enum.
* ``enum_field``: a list of the different fields of the enum. Each field is a
  list containing:

  * The name of the field.
  * A short description of the field.

Our game will use only one enum, ``error``, the return value of action
functions.

We can write it quickly::

  enum:
    -
      enum_name: error
      enum_summary:
        "Enumeration containing all possible error types that can be returned
        by action functions"
      enum_field:
        - [ok, "no error occurred"]
        - [out_of_bounds, "provided position is out of bounds"]
        - [full, "the selected column is full"]
        - [already_played, "you already played this turn, you cheater!"]


The structures
--------------

A structure is described by four fields:

* ``str_name``: the name of the structure.
* ``str_summary``: a short documentation of the structure
* ``str_tuple`` (yes/no): if set to "yes", in the languages that support it
  (Python and OCaml for instance), the structure will be represented as a tuple
  instead.
* ``str_field``: a list of the different fields of the struct. Each field is a
  list containing:

  * The name of the field.
  * The type of the field.
  * A short description of the field.

The only struct we'll need is a ``position { int x; int y; }``, to describe a
position in the board::

  struct:
    -
      str_name: position
      str_summary: Represents a position in the board
      str_tuple: yes
      str_field:
        - [x, int, "X coordinate (number of the column)"]
        - [y, int, "Y coordinate (number of the row)"]


The functions
-------------

The next part is to write the API that the player will use to play. The
functions are usually separated in three kinds:

* The observers: functions that the player can call to see the state of a game.
  They can take some parameters to describe the information that the player
  wants, and they return the desired information.
* The actions: functions that the player can call to perform some action. They
  usually take some parameters to describe how the action should be executed
  and return an error. Errors are generally represented by an enum you have to
  implement.
* The state modifiers: functions that can cancel some actions or modify the
  state of the game.

So, here are the observers we'll implement:

* ``my_player``: returns the ID of the current player
* ``get_board``: returns the board (a 2D int matrix corresponding to the jetons
  and their owners (-1 for "free", the id of the player else).
* ``cell``: returns the owner of the specified cell (-1 for "free").

The actions:

* ``drop``: drop a jeton at the specified column.

The modifiers:

* ``cancel``: cancel the last action.

Add this at the end::

  function:
    -
      fct_name: my_player
      fct_summary: Return your player number
      fct_ret_type: int
      fct_arg: []
    -
      fct_name: get_board
      fct_summary: Return the Connect4 board
      fct_ret_type: int array array
      fct_arg: []
    -
      fct_name: cell
      fct_summary: Return the player of a cell (-1 for "free")
      fct_ret_type: int
      fct_arg:
        - [pos, position, position of the cell]
    -
      fct_name: drop
      fct_summary: Drop a jeton at the given position
      fct_ret_type: error
      fct_arg:
        - [column, int, column where to drop a jeton]
    -
      fct_name: cancel
      fct_summary: Cancel the last played action
      fct_ret_type: bool
      fct_arg: []

And we're done !

Generate the skeleton
=====================

Stechec2 provides a script to generate a skeleton of the rules. It really saves
a lot of time, so don't skip this part !

If you have properly installed stechec2, you should have the generator in your
PATH::

  $ generator -h   # Display a lot of useful help
  $ generator rules ./connect4.yml gen
  $ mv gen/connect4/rules src
  $ rm -rf gen
  $ ls src
  api.cc  api.hh  constant.hh  entry.cc  interface.cc  rules.cc  rules.hh

You don't have to modify ``constant.hh``, ``entry.hh`` and ``interface.hh``.
They are generated files that shouldn't be manually edited.

The wscript
===========

Stechec2 uses the waf.py Makefile-like to build the games. We need to create a
``wscript`` file in the root folder of our game, containing this ::

  #! /usr/bin/env python2

  def options(opt):
      pass

  def configure(cfg):
      pass

  def build(bld):
      bld.shlib(
          source = '''
              src/rules.cc
              src/entry.cc
              src/interface.cc
              src/api.cc
          ''',
          defines = ['MODULE_COLOR=ANSI_COL_BROWN', 'MODULE_NAME="rules"'],
          target = 'connect4',
          use = ['stechec2'],
      )

      bld.install_files('${PREFIX}/share/stechec2/connect4', [
          'connect4.yml',
      ])

You can add source files to the ``source`` string. You don't need to change the
rest for now.


The rules
=========

The loops
---------

The first thing is to take a look at ``rules.cc`` and ``rules.hh``. There are
the three functions every rules should implement: ``client_loop``,
``spectator_loop`` and ``server_loop``. Writing these loops are painful: you
have to handle the turns, the phases, the order of each players… luckily
stechec2 provides some generic loops for some kind of games: ``TurnBasedRules``
and ``SynchronousRules``. We just have to inherit our Rules class from
TurnBasedRules, which matches the gameplay of the Connect4.


In ``rules.hh``:

* delete the methods ``client_loop``, ``server_loop`` and ``spectator_loop``.
* include ``rules/rules.hh`` at the top of your files (it contains the generic
  loops)
* make the class inherit from TurnBasedRules::

    class Rules : public rules::TurnBasedRules
    {

In ``rules.cc``:

* delete the methods ``client_loop``, ``server_loop`` and ``spectator_loop``.
* initialize TurnBasedRules with the options in the constructor::

    Rules::Rules(const rules::Options opt)
      : TurnBasedRules(opt)

If you're interested in how the generic loops work behind the scene, you can
take a look at ``stechec2/src/lib/rules/rules.hh``
