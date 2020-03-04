// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

#include <functional> // needed for std::hash

/// All possible error types that can be returned by action functions
typedef enum error
{
    OK,            /* <- no error occurred */
    OUT_OF_BOUNDS, /* <- guess is out of bounds */
} error;

// This is needed for old compilers
namespace std
{
template <>
struct hash<error>
{
    size_t operator()(const error& v) const
    {
        return hash<int>()(static_cast<int>(v));
    }
};
} // namespace std
