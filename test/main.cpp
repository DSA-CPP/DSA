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
    auto e2 = dsa::find_entry(ctx.entries(), 42069);
    assert(e == e2, "Entry of ID");
    assert(ctx.add().id() == invalid, "Empty Entry");
    e = dsa::find_entry(ctx.entries(), 42069);
    r = ctx.entries();
    assert(r.end() - *r.begin() == 12 && r.end() - 8 == e, "Add 2");
    assert(e.id() == 42069 && e[0] == 500 && e[1] == invalid && e[2] == invalid, "Entry");
    // save() & load()
}

static void server() {
    using dsa::server::file, dsa::server::all_files;
    namespace fs = std::filesystem;
    // participants() & emplace()
    assert(file(disc.id()).filename == "10.dsa", "Filename");
    fs::create_directory("testing");
    std::fstream{"testing/test.txt"};
    fs::current_path("testing");
    file(dsa::discipline_id{9, 9}).save({});
    file(disc.id()).save({});
    auto files = all_files();
    assert(files.size() == 1 &&
        files[0].first->id() == dsa::discipline_id{1, 0} &&
        files[0].second.filename == file(disc.id()).filename,
        "Identify files correctly");
    file(dsa::get_discipline({0, 0})->id()).save({});
    files = all_files();
    assert(files.size() == 2, "Identify all");
    fs::current_path("..");
    files = all_files("testing");
    assert(files.size() == 2, "Identify all from parent");
    fs::remove("testing/" + file(dsa::get_discipline({0, 0})->id()).filename);
    files = all_files("testing");
    assert(files.size() == 1 &&
        files[0].first->id() == dsa::discipline_id{1, 0} &&
        files[0].second.filename == file(disc.id()).filename,
        "Identify correctly from parent");
    fs::remove_all("testing");
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

struct range {
public:
    static constexpr auto size() noexcept { return discs.size(); }
    constexpr auto begin() const noexcept { return dsa::result_iterator{discs.begin(), vals.data()}; }
    constexpr auto end() const noexcept { return discs.end(); }
private:
    static constexpr std::array<dsa::discipline const *, 3> discs{{
        &dsa::disciplines[0],
        &dsa::disciplines[1],
        &dsa::disciplines[2]
    }};
    static constexpr std::array<dsa::entry_type, 5> vals{{
        1,
        invalid,
        17, invalid, invalid
    }};
};

static void individual_values(net::tcp::server const & s) {
    std::jthread st{[&]() {
        net::tcp::connection c{s};
        assert(dsa::detail::recv<dsa::entry_type>(c) == 0, "Request ID");
        dsa::server::send(c, range{}, range::size());
    }};
    dsa::client::discipline_results results{ep, 0};
    for(auto it = range{}.begin(); auto && [rd, re] : results) {
        assert((it != range{}.end()) == true, "Complete receive");
        auto && [td, te] = *it;
        assert(&rd == &td, "Correct discipline");
        assert(std::memcmp(re, te, rd.tries), "Correct values");
        ++it;
    }
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
    individual_values(server);
} catch(char const * e) {
    std::cout << "Failed at: " << e << std::endl;
    return -1;
}
