#ifndef ASCII_FORMAT_HPP
# define ASCII_FORMAT_HPP

/*
 * ANSI escape codes for text formatting
 */

# define FMT_RESET          "\033[0m"
# define FMT_BOLD           "\033[1m"
# define FMT_DIM            "\033[2m"

// Colors
# define FMT_BLACK          "\033[30m"
# define FMT_RED            "\033[31m"
# define FMT_GREEN          "\033[32m"
# define FMT_YELLOW         "\033[33m"
# define FMT_BLUE           "\033[34m"
# define FMT_MAGENTA        "\033[35m"
# define FMT_CYAN           "\033[36m"
# define FMT_WHITE          "\033[37m"

// Bright colors
# define FMT_BRIGHT_BLACK   "\033[90m"
# define FMT_BRIGHT_RED     "\033[91m"
# define FMT_BRIGHT_GREEN   "\033[92m"
# define FMT_BRIGHT_YELLOW  "\033[93m"
# define FMT_BRIGHT_BLUE    "\033[94m"
# define FMT_BRIGHT_MAGENTA "\033[95m"
# define FMT_BRIGHT_CYAN    "\033[96m"
# define FMT_BRIGHT_WHITE   "\033[97m"

// Background colors
# define FMT_BG_BLACK       "\033[40m"
# define FMT_BG_RED         "\033[41m"
# define FMT_BG_GREEN       "\033[42m"
# define FMT_BG_YELLOW      "\033[43m"
# define FMT_BG_BLUE        "\033[44m"
# define FMT_BG_MAGENTA     "\033[45m"
# define FMT_BG_CYAN        "\033[46m"
# define FMT_BG_WHITE       "\033[47m"

// Log levels
# define FMT_INFO           FMT_YELLOW  "[INFO]"    FMT_RESET
# define FMT_WARNING        FMT_MAGENTA "[WARNING]" FMT_RESET
# define FMT_ERROR          FMT_RED     "[ERROR]"   FMT_RESET
# define FMT_DONE           FMT_GREEN   "[DONE]"    FMT_RESET

#endif
