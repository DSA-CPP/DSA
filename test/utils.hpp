#include <array>
#include <iostream>
#include <string_view>
#include "client.hpp"
#include "server.hpp"

constexpr bool scmp(char const * s1, char const * s2) noexcept { return std::string_view{s1} == s2; }
constexpr void assert(bool pred, char const * error_message) { if(!pred) throw error_message; }

#define CATCH catch(char const * e) {\
    std::cout << "Failed at: " << e << std::endl;\
    return -1;\
}

inline constexpr auto ep = net::endpoint(0, 54321);
inline constexpr auto & disc = dsa::disciplines[2];
inline constexpr std::array<std::pair<dsa::entry_type, dsa::participant>, 2> parts{{
    {0, {17, false}},
    {1, {16, true}}
}};

inline constexpr std::array<dsa::entry_type, 8> entries{{
    0, 10, 20, 30,
    1, static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1)
}};
