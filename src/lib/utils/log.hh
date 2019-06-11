// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#pragma once

// Implementation of a logger based on the one in the first version of Stechec.

#include <cstdlib>
#include <iostream>
#include <ostream>

#include <gflags/gflags.h>

DECLARE_int32(verbose);

namespace utils {

#define ANSI_COL_NONE "\033[0m"
#define ANSI_COL_RED "\033[1;31m"
#define ANSI_COL_GREEN "\033[1;32m"
#define ANSI_COL_BROWN "\033[1;33m"
#define ANSI_COL_BLUE "\033[1;34m"
#define ANSI_COL_PURPLE "\033[1;35m"
#define ANSI_COL_CYAN "\033[1;36m"
#define ANSI_COL_GRAY "\033[0;30m"
#define ANSI_COL_BRED "\033[0;31m"
#define ANSI_COL_BGREEN "\033[0;32m"
#define ANSI_COL_YELLOW "\033[0;33m"
#define ANSI_COL_BBLUE "\033[0;34m"
#define ANSI_COL_BPURPLE "\033[0;35m"
#define ANSI_COL_BCYAN "\033[0;36m"

class Logger
{
public:
    enum DisplayLevel
    {
        FATAL_LEVEL = 0,
        ERROR_LEVEL = 1,
        WARNING_LEVEL = 2,
        INFO_LEVEL = 3,
        NOTICE_LEVEL = 4,
        DEBUG_LEVEL = 5
    };

    std::ostream& stream() { return std::cerr; }

    DisplayLevel& level() { return level_; }
    const DisplayLevel& level() const { return level_; }

    static Logger& get()
    {
        static Logger l;
        return l;
    }

private:
    Logger();
    DisplayLevel level_;
};

#ifndef MODULE_NAME
#define MODULE_NAME "unknown"
#endif

#ifndef MODULE_COLOR
#define MODULE_COLOR ANSI_COL_RED
#endif

void log(Logger::DisplayLevel lvl, const char* file, int line,
         const char* module_name, const char* module_color, const char* fmt,
         ...);

#define LOG(lvl, args...) \
    utils::log(lvl, __FILE__, __LINE__, MODULE_NAME, MODULE_COLOR, ##args)
#define ERR(args...) LOG(utils::Logger::ERROR_LEVEL, ##args)
#define WARN(args...) LOG(utils::Logger::WARNING_LEVEL, ##args)
#define INFO(args...) LOG(utils::Logger::INFO_LEVEL, ##args)
#define NOTICE(args...) LOG(utils::Logger::NOTICE_LEVEL, ##args)
#define DEBUG(args...) LOG(utils::Logger::DEBUG_LEVEL, ##args)

#define FATAL(args...) \
    do \
    { \
        LOG(utils::Logger::FATAL_LEVEL, ##args); \
        abort(); \
    } while (0)

// Basically, an assertion using our logging system
#define CHECK(cond) \
    do \
    { \
        if (!(cond)) \
            FATAL("%s", "Assertion failed: " #cond " -- aborting"); \
    } while (0)

// An assertion which raises an exception instead of aborting.
#define CHECK_EXC(exc, cond) \
    do \
    { \
        if (!(cond)) \
        { \
            ERR("%s", "Assertion failed: " #cond " -- raising " #exc); \
            throw exc(); \
        } \
    } while (0)

} // namespace utils
