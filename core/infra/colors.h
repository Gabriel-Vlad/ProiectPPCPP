#pragma once

namespace Color {
    inline constexpr const char* RESET  = "\033[0m";   // back to the default style
    inline constexpr const char* BOLD   = "\033[1m";   // bold text
    inline constexpr const char* RED    = "\033[31m";  // errors
    inline constexpr const char* GREEN  = "\033[32m";  // success messages
    inline constexpr const char* YELLOW = "\033[33m";  // section titles, percentages
    inline constexpr const char* CYAN   = "\033[36m";  // highlighted values (ids, counts)
    inline constexpr const char* MUTED  = "\033[90m";  // secondary, less important text
}
