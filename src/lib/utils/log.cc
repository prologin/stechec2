// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "log.hh"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <gflags/gflags.h>

DEFINE_int32(verbose, 5, "Verbosity of the log (0-5)");

namespace utils
{

Logger::Logger()
    : level_(static_cast<Logger::DisplayLevel>(FLAGS_verbose))
{
}

void log(Logger::DisplayLevel lvl, const char* file, int line,
         const char* module_name, const char* module_color, const char* fmt,
         ...)
{
    // Check if the current verbosity level is enough
    if (lvl > Logger::get().level())
        return;

    va_list va;
    va_start(va, fmt);

    char buffer[4096];
    char fmt_buffer[4096];

    // Only display the basename of the file, not the full path
    const char* filename = strrchr(file, '/');
    if (filename)
        file = filename + 1;

    // Format the log message
    snprintf(fmt_buffer, sizeof(fmt_buffer), "[%s%s%s] %s (%s:%d)",
             module_color, module_name, ANSI_COL_NONE, fmt, file, line);
    vsnprintf(buffer, sizeof(buffer), fmt_buffer, va);

    Logger::get().stream() << buffer << std::endl;

    va_end(va);
}

} // namespace utils
