// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <rules/client-messenger.hh>
#include <rules/options.hh>
#include <rules/server-messenger.hh>

namespace rules {

using f_rules_init = void (*)(const Options&);
using f_rules_result = void (*)();

using f_client_loop = void (*)(ClientMessenger_sptr);
using f_server_loop = void (*)(ServerMessenger_sptr);

} // namespace rules
