#include <filesystem>
#include <iostream>
#include <string_view>
#include "dsa.hpp"
#ifdef _CLIENT
#include <thread>
#include "client.hpp"
#endif
#ifdef _SERVER
#include "server.hpp"
#endif

constexpr bool scmp(char const * s1, char const * s2) noexcept { return std::string_view{s1} == s2; }
constexpr void assert(bool pred, char const * error_message) { if(!pred) throw error_message; }

inline void io() {
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

constexpr void formatter() {
    using dsa::format, dsa::score;
    dsa::formatter f{format::NONE};
    char buf[20];
    dsa::entry_type values[]{10, 30, 20}, invalid{static_cast<dsa::entry_type>(-1)}, values2[]{invalid, 10, invalid};

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

constexpr void disciplines() {
    assert(dsa::name(dsa::disciplines[2]) == "KraftWerfen", "Discipline Name");
}

constexpr void participants() {
    for(int i{10}; i <= 21; ++i)
        assert(dsa::participant::valid_age(i), "Valid Age");
    for(auto i : {9, 23})
        assert(!dsa::participant::valid_age(i), "Invalid Age");
}

static void context() {
#ifdef _CLIENT
    using dsa::disciplines;
    auto invalid = static_cast<dsa::entry_type>(-1);
    dsa::client::context ctx{"Test-Station"};
    assert(ctx.name() == "Test-Station", "New Context");
    assert(!ctx.current(), "Empty Context");
    auto && disc = disciplines[2];
    static_assert(dsa::name(disc) == "KraftWerfen");
    ctx.load(disc);
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
    // todo: test participants
    auto ep = net::endpoint(0, 54321);
    std::pair<dsa::entry_type, dsa::participant> vals[]{
        {0, {17, false}},
        {1, {16, true}}
    };
    NET_INIT();
    std::jthread s_t{[&]() {
        net::tcp::server serv{ep, 1};
        net::tcp::connection conn{serv};
        conn.sendall({reinterpret_cast<char *>(vals), sizeof(vals)});
    }};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ctx.load_participants({ep});
    assert(ctx.participants().size() == 2, "Load all");
    for(auto && [id, part] : vals) {
        auto && p = ctx.participants().at(id);
        assert(p.age == part.age && p.male == part.male, "Load correctly");
    }
#endif
#ifdef _SERVER
#endif
}

int main() {
    try {
        io();
        formatter();
        disciplines();
        participants();
        context();
    } catch(char const * e) {
        std::cout << "Failed at: " << e << std::endl;
        return -1;
    }
}
