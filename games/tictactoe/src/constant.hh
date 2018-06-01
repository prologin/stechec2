/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) 2018 Prologin
*/

#ifndef CONSTANT_HH_
#define CONSTANT_HH_

#include <functional> // needed for std::hash

/// All possible error types that can be returned by action functions
typedef enum error {
    OK,               /* <- no error occurred */
    OUT_OF_BOUNDS,    /* <- provided position is out of bounds */
    ALREADY_OCCUPIED, /* <- someone already played at the provided position */
    ALREADY_PLAYED,   /* <- you already played this turn, you cheater! */
} error;
// This is needed for old compilers
namespace std
{
template <> struct hash<error>
{
    size_t operator()(const error& v) const
    {
        return hash<int>()(static_cast<int>(v));
    }
};
}

/// Position on the TicTacToe board
typedef struct position
{
    int x; /* <- X coordinate */
    int y; /* <- Y coordinate */
} position;

#endif // !CONSTANT_HH_
