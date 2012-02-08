#ifndef LIB_UTILS_LOG_HH_
# define LIB_UTILS_LOG_HH_

// Implementation of a logger based on the one in the first version of Stechec.

# include <iostream>
# include <ostream>

# define ANSI_COL_NONE        "\033[0m"
# define ANSI_COL_RED         "\033[1;31m"
# define ANSI_COL_GREEN       "\033[1;32m"
# define ANSI_COL_BROWN       "\033[1;33m"
# define ANSI_COL_BLUE        "\033[1;34m"
# define ANSI_COL_PURPLE      "\033[1;35m"
# define ANSI_COL_CYAN        "\033[1;36m"
# define ANSI_COL_GRAY        "\033[0;30m"
# define ANSI_COL_BRED        "\033[0;31m"
# define ANSI_COL_BGREEN      "\033[0;32m"
# define ANSI_COL_YELLOW      "\033[0;33m"
# define ANSI_COL_BBLUE       "\033[0;34m"
# define ANSI_COL_BPURPLE     "\033[0;35m"
# define ANSI_COL_BCYAN       "\033[0;36m"

class Logger
{
public:
    enum DisplayLevel
    {
        ERROR_LEVEL = 0,
        WARNING_LEVEL = 1,
        INFO_LEVEL = 2,
        NOTICE_LEVEL = 3
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
    Logger(DisplayLevel level = WARNING_LEVEL) : level_(level) {}
    DisplayLevel level_;
};

# ifndef MODULE_NAME
#  define MODULE_NAME "unknown"
# endif

# ifndef MODULE_COLOR
#  define MODULE_COLOR ANSI_COL_RED
# endif

void log(Logger::DisplayLevel lvl, const char* file, int line,
         const char* module_name, const char* module_color,
         const char* fmt, ...);

# define LOG(lvl, fmt, args...) ::log(lvl, __FILE__, __LINE__, \
                                           MODULE_NAME, MODULE_COLOR, \
                                           fmt, ## args)
# define ERR(fmt, args...) LOG(::Logger::ERROR_LEVEL, fmt, ## args)
# define WARN(fmt, args...) LOG(::Logger::WARNING_LEVEL, fmt, ## args)
# define INFO(fmt, args...) LOG(::Logger::INFO_LEVEL, fmt, ## args)
# define NOTICE(fmt, args...) LOG(::Logger::NOTICE_LEVEL, fmt, ## args)

#endif // !LIB_UTILS_LOG_HH_
