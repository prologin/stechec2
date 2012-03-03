/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) 2005, 2006 Prologin
*/

#include <cstdlib>
/* These functions are defined in common stechec rules. */
extern "C" int api_state_is_end(void*);
extern "C" int api_state_is_playturn(void*);
extern "C" void api_do_end_turn(void*);
extern "C" int api_get_nb_team(void*);
extern "C" int client_cx_process(void*);

/* Must be defined in champion library. */
extern "C" void init_game();
extern "C" void end_game();
extern "C" void jouer();

// prolo2010 specific : players do not execute simultaneously
extern "C" bool api_mon_tour();

extern "C" int api_send_actions();

extern "C" int run(void* foo, void* api, void* client_cx)
{
  init_game();
  while (!api_state_is_end(api))
    {
      // Wait until we can play.
      while (!api_state_is_playturn(api) && !api_state_is_end(api))
	client_cx_process(client_cx);
      if (api_state_is_end(api))
        break;

      if (api_mon_tour())
      {
	jouer();
      }
      api_send_actions();
      api_do_end_turn(api);
    }

  end_game();
  foo = foo;
  return 0;
}
