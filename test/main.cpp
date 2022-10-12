#include <array>
#include <cstring>
#include <iostream>
#include <string_view>
#include <thread>
#include "client.hpp"
#include "server.hpp"

inline constexpr auto invalid = static_cast<dsa::entry_type>(-1);
inline constexpr net::socket_address ep{net::endpoint(0, 54321)};
inline constexpr auto & disc = dsa::disciplines[2];
constexpr bool scmp(char const * s1, char const * s2) noexcept { return std::string_view{s1} == s2; }
constexpr void assert(bool pred, char const * error_message) { if(!pred) throw error_message; }

static void io() {
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
}

constexpr void format() {
    using dsa::format, dsa::score;
    dsa::formatter f{format::NONE};
    char buf[20];
    dsa::entry_type values[]{10, 30, 20},
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
}

static void client() {
    static_assert(dsa::name(disc.id()) == "KraftWerfen");
    auto invalid = static_cast<dsa::entry_type>(-1);
    dsa::client::context ctx{"Test-Station"};
    assert(ctx.name() == "Test-Station", "New Context");
    assert(!ctx.current(), "Empty Context");
    ctx.load(disc);
    assert(ctx.current(), "Non-Empty Context");
    auto r = ctx.entries();
    assert(*r.begin() == r.end(), "Empty Entries");
    auto e = (ctx.add(), ctx.add());
    e.id() = 42069;
    e[0] = 500;
    r = ctx.entries();
    assert(r.end() - *r.begin() == 8 && r.end() - 4 == e, "Add");
    auto e2 = ctx.entry_of(42069);
    assert(e == e2, "Entry of ID");
    assert(ctx.add().id() == invalid, "Empty Entry");
    e = ctx.entry_of(42069);
    r = ctx.entries();
    assert(r.end() - *r.begin() == 12 && r.end() - 8 == e, "Add 2");
    assert(e.id() == 42069 && e[0] == 500 && e[1] == invalid && e[2] == invalid, "Entry");
    // save() & load()
}

static void server() {
    // participants() & emplace()
    assert(dsa::server::filename(disc.id()) == "10.dsa", "Filename");
}

class server_status {
public:
    server_status(std::string_view sv) noexcept
    : sv_{sv} {
        std::cout << sv << '\r' << std::flush;
    }

    ~server_status() {
        for(auto s = sv_.size(); s--;)
            std::cout << ' ';
        std::cout << '\r' << std::flush;
    }

private:
    std::string_view sv_;
};

static void participants(net::tcp::server const & s) {
    static constinit std::array<std::pair<dsa::entry_type, dsa::participant>, 2> parts{{
        {0, {17, false}},
        {1, {16, true}}
    }};
    std::jthread st{[&]() {
        server_status ss{"participants() ..."};
        dsa::server::context ctx;
        for(auto && [id, info] : parts)
            ctx.emplace(id, info, {});
        ctx.send_participants(s);
    }};
    dsa::client::context ctx{"Test-Station 2"};
    ctx.load_participants(ep);
    assert(ctx.participants().size() == parts.size(), "Load all");
    for(auto && [id, part] : parts) {
        auto && p = ctx.participants().at(id);
        assert(p.age == part.age && p.male == part.male, "Load correctly");
    }
}

static void values(net::tcp::server const & s) {
    static constinit std::array<dsa::entry_type, 8> entries{{
        0, 10, 20, 30,
        1, invalid, invalid, invalid
    }};
    std::jthread st{[&]() {
        server_status ss{"values() ..."};
        auto [d, e] = dsa::receive_values(s);
        assert(d == disc.id() && e.size() == entries.size() &&
            !std::memcmp(e.data(), entries.data(), entries.size() * sizeof(dsa::entry_type)),
            "Receive entries");
    }};
    dsa::send_values(ep, disc.id(), entries);
}

int main() try {
    io();
    format();
    client();
    server();
    NET_INIT();
    net::tcp::server server{ep, 1};
    participants(server);
    values(server);
} catch(char const * e) {
    std::cout << "Failed at: " << e << std::endl;
    return -1;
}
