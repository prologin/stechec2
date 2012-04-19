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

#ifndef CONSTANT_HH_
# define CONSTANT_HH_

///
// Enumeration containing all possible error types that can be returned by action functions
//
typedef enum error {
  OK, /* <- no error occurred */
  OUT_OF_BOUNDS, /* <- provided position is out of bounds */
  ALREADY_OCCUPIED, /* <- someone already played at the provided position */
  ALREADY_PLAYED, /* <- you already played this turn, you cheater! */
} error;



#endif // !CONSTANT_HH_
