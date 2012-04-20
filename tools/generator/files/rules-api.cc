/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) !!year!! !!provider!!
*/

#include <cstdlib>
#include <net/client-messenger.hh>
#include <net/server-messenger.hh>

#include "rules.hh"

Rules* rules;

extern "C" {

void rules_init(const std::string& champion)
{
    rules = new Rules(champion);

    // FIXME
}

void rules_result()
{
    // FIXME

    delete rules;
}

void client_loop(net::ClientMessenger_sptr msgr)
{
    // FIXME
    abort();
}

void server_loop(net::ServerMessenger_sptr msgr)
{
    // FIXME
    abort();
}

} // extern "C"
