#include <array>
#include <iostream>
#include <string_view>
#include "client.hpp"
#include "server.hpp"

constexpr bool scmp(char const * s1, char const * s2) noexcept { return std::string_view{s1} == s2; }
constexpr void assert(bool pred, char const * error_message) { if(!pred) throw error_message; }
static void test();

int main() try {
    dsa::io<int> io{"test.dat"};
    auto vec = io.load();
    assert(vec.empty(), "Load 1");
    vec.assign({1, 2, 3});
    io.load(vec);
    assert(vec.empty(), "Load 2");
    vec.assign({1, 2, 3});
    io.save(vec);
    assert(io.load() == vec, "Save & Load");
    vec.resize(vec.size() + 2);
    io.save(vec);
    assert(io.load() == vec, "Resave");
    std::filesystem::remove("test.dat");

    using dsa::format, dsa::score;
    dsa::formatter f{format::NONE};
    char buf[20];
    dsa::entry_type values[]{10, 30, 20},
        invalid{static_cast<dsa::entry_type>(-1)},
        values2[]{invalid, 10, invalid};
    // ascending
    f = format::NONE;
    assert(f(values, {10, 20, 30}) == score::GOLD, "Eval 1 (ASC)");
    assert(f(values, {10, 30, 50}) == score::SILVER, "Eval 2 (ASC)");
    assert(f(values, {10, 50, 90}) == score::BRONCE, "Eval 3 (ASC)");
    assert(f(values, {50, 100, 150}) == score::NOTHING, "Eval 4 (ASC)");
    assert(f(values2, {0, 10, 20}) == score::SILVER, "Eval 5 (ASC)");
    // descending
    f = format::MINUTES;
    assert(f(values, {30, 20, 10}) == score::GOLD, "Eval 1 (DESC)");
    assert(f(values, {15, 10, 5}) == score::SILVER, "Eval 2 (DESC)");
    assert(f(values, {10, 5, 0}) == score::BRONCE, "Eval 3 (DESC)");
    assert(f(values, {5, 3, 1}) == score::NOTHING, "Eval 4 (DESC)");
    assert(f(values2, {20, 10, 0}) == score::SILVER, "Eval 5 (DESC)");
    // NONE
    f = format::NONE;
    assert(f("1234") == 1234, "String to Value (NONE)");
    assert(scmp((f(1234, buf), buf), "1234"), "Value to String (NONE)");
    // METERS
    f = format::METERS;
    assert(f("12,34m") == 1234, "String to Value (METERS)");
    assert(scmp((f(1234, buf), buf), "12,34"), "Value to String (METERS)");
    // MINUTES
    f = format::MINUTES;
    assert(f("12:34 min") == 1234, "String to Value (MINUTES)");
    assert(scmp((f(1234, buf), buf), "12:34"), "Value to String (MINUTES)");
    // SECONDS
    f = format::SECONDS;
    assert(f("123.4s") == 1234, "String to Value (SECONDS)");
    assert(scmp((f(1234, buf), buf), "123,4"), "Value to String (SECONDS)");

    test();
} catch(char const * e) {\
    std::cout << "Failed at: " << e << std::endl;\
    return -1;\
}

inline constexpr net::socket_address ep{net::endpoint(0, 54321)};
inline constexpr auto & disc = dsa::disciplines[2];
inline constexpr std::array<std::pair<dsa::entry_type, dsa::participant>, 2> parts{{
    {0, {17, false}},
    {1, {16, true}}
}};

inline constexpr std::array<dsa::entry_type, 8> entries{{
    0, 10, 20, 30,
    1, static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1)
}};
