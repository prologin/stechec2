// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <utils/log.hh>
#include <utils/versioned_ptr.hh>

namespace rules {

class GameState
{
public:
    virtual ~GameState() {}
};

} // namespace rules
