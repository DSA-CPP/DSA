#include <filesystem>
#include <iostream>
#include <string>
#include "disciplines.hpp"

constexpr bool strcmp(char const * s1, char const * s2) noexcept { return std::string{s1} == s2; }
constexpr void assert(bool pred, char const * error_message) { if(!pred) throw error_message; }

inline void io() {
    dsa::io<dsa::entry_type, 9> io{"test.dat"};
    auto vec = io.load();
    assert(vec.empty(), "Load");
    vec.assign({69, 420});
    io.save(vec);
    assert(io.load() == vec, "Save Vector");
    vec.resize(vec.size() + 2);
    io.save(vec);
    assert(io.load() == vec, "Resave Entry");
    std::filesystem::remove("test.dat");
}

constexpr void formatter() {
    using dsa::score, dsa::format;
    dsa::formatter f = format::NONE;
    char buf[20];
    dsa::entry_type values[3] {10, 20, 30};
    dsa::entry_type invalids[3] {static_cast<dsa::entry_type>(-1), 10, static_cast<dsa::entry_type>(-1)};
    // all
    assert(f("1234") == 1234, "String to Value");
    // ascending
    f = format::NONE;
    assert(f(values, {10, 20, 30}) == score::GOLD, "Eval 1 (ASC)");
    assert(f(values, {10, 30, 50}) == score::SILVER, "Eval 2 (ASC)");
    assert(f(values, {10, 50, 90}) == score::BRONCE, "Eval 3 (ASC)");
    assert(f(values, {50, 100, 150}) == score::NOTHING, "Eval 4 (ASC)");
    assert(f(invalids, {0, 10, 20}) == score::SILVER, "Eval 5 (ASC)");
    // descending
    f = format::MINUTES;
    assert(f(values, {30, 20, 10}) == score::GOLD, "Eval 1 (DESC)");
    assert(f(values, {15, 10, 5}) == score::SILVER, "Eval 2 (DESC)");
    assert(f(values, {10, 5, 0}) == score::BRONCE, "Eval 3 (DESC)");
    assert(f(values, {5, 3, 1}) == score::NOTHING, "Eval 4 (DESC)");
    assert(f(invalids, {20, 10, 0}) == score::SILVER, "Eval 5 (DESC)");
    // NONE
    f = format::NONE;
    f(1234, buf);
    assert(strcmp(buf, "1234"), "Value to String (NONE)");
    // METERS
    f = format::METERS;
    f(1234, buf);
    assert(strcmp(buf, "12,34"), "Value to String (METERS)");
    // MINUTES
    f = format::MINUTES;
    f(1234, buf);
    assert(strcmp(buf, "12:34"), "Value to String (MINUTES)");
    // SECONDS
    f = format::SECONDS;
    f(1234, buf);
    assert(strcmp(buf, "123,4"), "Value to String (SECONDS)");
}

constexpr void disciplines() {
    using dsa::entry_type, dsa::disciplines;
    assert(strcmp(disciplines[2].io.filename, "KraftWerfen.dsa"), "Filename");
    auto && disc = disciplines[2].discipline;
    auto e = (disc.add(), disc.add());
    e.id() = 42069;
    e[0] = 500;
    assert(disc.end() - *disc.begin() == 8 && disc.end() - 4 == e, "Add");
    auto e2 = disc.entry_of(42069);
    assert(e2 == e, "ID");
    assert(disc.add().id() == static_cast<entry_type>(-1), "Empty Entry");
    e = disc.entry_of(42069);
    assert(disc.end() - *disc.begin() == 12 && disc.end() - 8 == e, "Add 2");
    assert(e.id() == 42069 && e[0] == 500 && e[1] == static_cast<entry_type>(-1), "Entry");
    for(entry_type * ptr{*disc.begin()}; auto && entry : disc) assert(ptr == entry, "Iterator"), ptr += 4;
    dsa::discipline d{disc};
    for(d = {}; auto && e : d) assert(e && false, "Assign"); // avoid warning, no cost and also in test
}

void participants() {
    using dsa::participant, dsa::participants, dsa::age_valid;
    assert(participants.size() == 0, "Empty");
    participants.insert({
        {  69, {16,  true}},
        { 420, {17,  true}},
        {1337, {14, false}}
    }); // non-const test
    // age_valid & levels_for not testable
}

int main() {
    try {
        io();
        formatter();
        disciplines();
        participants();
    } catch(char const * e) {
        std::cout << "Failed at: " << e << std::endl;
        return -1;
    }
}
