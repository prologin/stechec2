/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) 2012 Prologin
*/

#include <cstdlib>
#include <rules/options.hh>
#include <net/client-messenger.hh>
#include <net/server-messenger.hh>

#include "rules.hh"

static Rules* rules_;

extern "C" {

void rules_init(const rules::Options& opt)
{
    rules_ = new Rules(opt);

    // FIXME
}

void rules_result()
{
    // FIXME

    delete rules_;
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
