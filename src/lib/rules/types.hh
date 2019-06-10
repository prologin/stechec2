// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#pragma once

#include <rules/client-messenger.hh>
#include <rules/options.hh>
#include <rules/server-messenger.hh>

namespace rules {

typedef void (*f_rules_init)(const Options&);
typedef void (*f_rules_result)();

typedef void (*f_client_loop)(ClientMessenger_sptr);
typedef void (*f_server_loop)(ServerMessenger_sptr);

} // namespace rules
