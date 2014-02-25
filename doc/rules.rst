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
game, only one action (drop a disk somewhere).

We can begin to describe the rules in the .yml:

.. code-block:: bash

  $ mkdir connect4
  $ cd connect4
  $ vim connect4.yml

Write the YAML
==============

.. highlight:: yaml

First, we must write some config boilerplate at the top of the file::

  name: connect4  # The name of the game

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
* ``get_column``: returns the column (a int array corresponding to the disks
  of a column and their owners (-1 for "free", the id of the player else).
* ``cell``: returns the owner of the specified cell (-1 for "free").

The actions:

* ``drop``: drop a disk at the specified column.

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
      fct_name: get_column
      fct_summary: Return the column
      fct_ret_type: int array
      fct_arg:
        - [number, int, number of the column]
    -
      fct_name: cell
      fct_summary: Return the player of a cell (-1 for "free")
      fct_ret_type: int
      fct_arg:
        - [pos, position, position of the cell]
    -
      fct_name: drop
      fct_summary: Drop a disk at the given position
      fct_ret_type: error
      fct_arg:
        - [column, int, column where to drop a disk]
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
PATH:

.. code-block:: bash

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
``wscript`` file in the root folder of our game, containing this:

.. code-block:: python

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

.. highlight:: cpp

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
take a look at ``stechec2/src/lib/rules/rules.hh``.

We we'll come back to this code later, but for now if we want it to compile, we
should first add this::

    #include <rules/actions.hh> // At the top of the file

and::

    virtual rules::Actions* get_actions() { return NULL; }
    virtual void apply_action(const rules::IAction_sptr&) {}
    virtual bool is_finished() { return true; }

This is of course just a temporary fix to allow us to compile the code.


The game-state
--------------

We need to have a gamestate class which will contain the state of the game, and
which we can interact with (the methods of this class will change the state of
the game.) The majority of this part will be left as an exercise for the
reader.

The GameState will be located in ``game.cc`` and ``game.hh``. Don't forget to
add those files to the ``wscript``.

The GameState should inherit from rules::GameState (``#include
<rules/game-state.hh>``), have a copy constructor and a destructor, and
override a ``copy()`` method. You'll also have ``get_current_turn`` and
``increment_turn`` which will do the needful with an internal counter, a
``get_board`` method which will return the 2D board, a ``drop`` to drop a
disk somewhere (returns true if the disk has been successfuly dropped), a
``is_full`` to check if one can play in a specific column, and finally, a
``winner`` method which will return the winner if there's one, -1 else.

Here's a template of the functions you'll need to implement::

    GameState(rules::Players_sptr players);
    GameState(const GameState& st);
    ~GameState();
    virtual rules::GameState* copy() const;

    void increment_turn();

    int get_current_turn() const;
    bool is_full(int column) const;
    std::array<std::array<int, NB_COLS>, NB_ROWS> get_board() const;
    int winner() const;

    void drop(int column, int player);

Testing
-------

Making unit test bit by bit as your rules are becoming more and more complex is
really important: you don't want to test all the possible cases with custom
champions.

Let's create a ``src/tests`` folder, where we'll put all our test files. The
tests use googletest, you can find a reference documentation
[here](http://code.google.com/p/googletest/).

Here, we're going to create a ``test-gamestate.cc`` to test that the functions
we just created are working well.

Here's a template for ``test-game.cc``::

    #include <gtest/gtest.h>
    #include "../game.hh"

    class GameStateTest : public ::testing::Test
    {
        protected:
            virtual void SetUp()
            {
                // Some code that will be executed before each test

                // Create an array of two players
                rules::Players_sptr players(new rules::Players {
                        std::vector<rules::Player_sptr> {
                            rules::Player_sptr(new rules::Player(0, 0)),
                            rules::Player_sptr(new rules::Player(1, 0)),
                        }
                    }
                );

                gamestate_ = new GameState(players);
            }

            GameState* gamestate_;
    };

    TEST_F(GameStateTest, TestName)
    {
        // Test content
    }

You can then create as many tests as you want, for instance::

    TEST_F(GameStateTest, CheckDropOverflow)
    {
        for (int i = 0; i < NB_ROWS; i++)
        {
            ASSERT_EQ(gamestate_->is_full(0), false);
            gamestate_->drop(0, 0);
        }
        ASSERT_EQ(gamestate_->is_full(0), true);
    }

Create the following tests:

* *CheckFull*: checks that ``is_full`` returns ``true`` when the column
  is full

* **CheckDrop**: checks that the board obtained by dropping disks is valid

* **CheckWinner**: checks that you winner() function works correctly

To run the tests, you just have to build using the ``--check`` option:

.. code-block:: bash

  ./waf.py build --check


The actions
-----------

The actions are the only objects sent on the network. Let me expand on that
part a bit. When you run a stechec2 match, you have a server and two clients.
They load the same shared library that defines the rules of the game, and they
create a local GameState (actually a linked list of gamestates, to allow a
cancel() action that undoes actions). When a player wants to perform an action,
the rules first check if the action can be made considering the current state
of the game. If everything is okay, the stechec2 client "apply" the action to
the gamestate and send the action over the network. The server then receives
the action, and check if it can be made too. If not, there's a big
synchronisation problem (or possibly an attack), so the server disconnects the
client. Else, the server applies the action locally to his gamestate and
broadcast the action to the other players (so that they can do the same with
their gamestates).

An action must define five functions that will be used by the rules:

* **check(gamestate)**: checks that the action can be applied on the gamestate ;
* **apply_on(gamestate)**: applies the action to the given gamestate ;
* **handle_buffer(buffer)**: used to serialize the action object to a buffer ;
* **id()**: returns the ID of the action (usually an element of an enum) ;
* **player_id()**: returns the ID of the player that sent the action ;

For now we just have one action, so we can just put that in a new file called
``actions.hh``::

    enum action_id {
        ID_ACTION_DROP,
    }

Now here's a template of the ``drop`` action class you're going to implement in
``action-drop.{cc,hh}``::

    # include <rules/action.hh>

    # include "constant.hh"
    # include "game.hh"
    # include "actions.hh"

    class ActionDrop : public rules::Action<GameState>
    {
        public:
            ActionDrop(int player /* ... */);

            /* Necessary later, init attributes to random values */
            ActionDrop();

            virtual int check(const GameState*) const;
            virtual void handle_buffer(utils::Buffer&);

            uint32_t player_id() const { return player_; }
            uint32_t id() const { return ID_ACTION_DROP; }

        protected:
            virtual void apply_on(GameState*) const;

        protected:
            int player_;
            /* ... */
    };


Note that:

* **check** should return an element of the error enumeration we've defined in
  the rules (see ``constant.hh``) : { OK, OUT_OF_BOUNDS, FULL, ALREADY_PLAYED }

* **handle_buffer** only has to "bufferize" each attribute of the object. For
  that, use the ``buf.handle()`` function like this::

    buf.handle(player_);
    buf.handle(id_);
    buf.handle(/* Any kind of simple type */);
    buf.handle_array(/* Array */);

The API
-------

In the bunch of files you've previously generated, there is a file called
``api.cc`` that will describe what happens when the player calls a function
during the game. These functions are directly "translated" in the language from
which they are calling them, so you just have to implement the behaviour as if
everyone played in C++.

The observers are a realy easy part, you just have to return some values from
the GameState and the rules::Player objects. For instance with my_player::

    int Api::my_player()
    {
        return player_->id;
    }

Implement all the other observers : ``get_column`` and ``get_cell``. You'll
have to replace ``rules::GameState`` to ``GameState`` (the one defined in
``game.cc``) in ``api.hh`` in order to be able to call our gamestate-specific
functions.

The ``cancel`` function is already implemented in stechec2. To call it you just
have to do this::

    bool Api::cancel()
    {
        if (!game_state_->can_cancel())
            return false;

        game_state_ = rules::cancel(game_state_);

        return true;
    }

Internally, there's a linked list of gamestates. The ``rules::cancel`` function
simply removes the current gamestate and returns the last.

The actions are more difficult to implement. The simplest solution is this one
: during each turn, you keep a ``rules::Actions`` object in your ``Api``
object, and each time a player executes an action, it will be locally applied
to the gamestate, and then send to the server at the end of the turn.

So let's add a ``rules::Actions`` attribute to our class, and a getter that
returns a reference to this object.

We also have to add a getter and a setter for the game_state : we can't just
replace it, it would prevent to use cancel(), so we have to use the
rules::GameState API::

    GameState* game_state() { return game_state_; }
    void game_state_set(rules::GameState* gs)
        { game_state_ = dynamic_cast<GameState*>(gs); }

Now we can implement ``Api::drop``. We first have to instanciate a
``rules::IAction_sptr`` using our action constructor. Then we retrieves the
result of ``check`` and cast it into an ``error`` (since ``check`` returns an
int). If the result is ``OK``, we apply the action locally and add it to the
Actions object and return ``OK``, else, we return the error::

    error Api::drop(int column)
    {
        rules::IAction_sptr action(new ActionDrop(column, player_->id));

        error err;
        if ((err = static_cast<error>(action->check(game_state_))) != OK)
            return err;
        actions_.add(action);
        game_state_set(action->apply(game_state()));
        return OK;
    }


The rules object
----------------

Let's typedef the function that will be called by the player as void*()'s in
our rules.hh::

    typedef void (*f_champion_init_game)();
    typedef void (*f_champion_play_turn)();
    typedef void (*f_champion_end_game)();


Then add these attributes to the Rules class::

    protected:
        f_champion_init_game champion_init_game;
        f_champion_play_turn champion_play_turn;
        f_champion_end_game champion_end_game;

    private:
        utils::DLL* champion_;
        Api* api_;
        utils::Sandbox sandbox_;


In the ``Rules`` constructor, we have to initialize a few objects, register
some actions and retrieve the champion library::

    Rules::Rules(const rules::Options opt)
        : TurnBasedRules(opt)
    {
        if (!opt.champion_lib.empty())
            champion_dll_ = new utils::DLL(opt.champion_lib);
        else
            champion_dll_ = nullptr;

        // Init the gamestate
        GameState* game_state = new GameState(opt.players);

        // Init the API
        api_ = new Api(game_state, opt.player);

        // If we are a client, retrieves the functions from the champion
        // library
        if (!opt.champion_lib.empty())
        {
            champion_dll_ = new utils::DLL(opt.champion_lib);
            champion_init_game =
                champion_dll_->get<f_champion_init_game>("init_game");
            champion_play_turn =
                champion_dll_->get<f_champion_play_turn>("play_turn");
            champion_end_game =
                champion_dll_->get<f_champion_end_game>("end_game");
        }

        // Register the actions
        api_->actions()->register_action(ID_ACTION_DROP,
                []() -> rules::IAction* { return new ActionDrop(); });

    }

    Rules::~Rules()
    {
        delete champion_dll_;
        delete api_;
    }

Then we can overload the functions defined in ``<rules/rules.hh>`` to satisfy
our needs. For instance, we want to overload ``at_client_start``,
``player_turn`` and ``at_client_end`` to execute the init_game and end_game
client functions. To do so, we'll use the sandbox object::

    void Rules::at_client_start()
    {
        sandbox_.execute(champion_partie_init);
    }

We also have to implement ``get_actions``, ``apply_action`` and ``is_finished``
as previously defined in ``rules.hh``::

    rules::Actions* Rules::get_actions()
    {
        return api_->actions();
    }

    void Rules::apply_action(const rules::IAction_sptr& action)
    {
        api_->game_state_set(action->apply(api_->game_state()));
    }

At the end of each turn, we have to check if someone has won, by overloading
``end_of_player_turn``, and doing the needful. Then, you can implement
``is_finished``.

And that's it !
