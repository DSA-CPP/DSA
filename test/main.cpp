#include <filesystem>
#include <iostream>
#include <string>
#include "dsa.hpp"

constexpr bool strcmp(char const * s1, char const * s2) noexcept { return std::string{s1} == s2; }
constexpr void assert(bool pred, char const * error_message) { if(!pred) throw error_message; }

inline void io() {
    dsa::io<dsa::entry_type> io{"test.dat"};
    auto vec = io.load();
    assert(vec.empty(), "Load");
    vec.assign({69, 420});
    io.save(vec);
    auto v = io.load();
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
    { // all
        assert(f("1234") == 1234, "String to Value");
    }
    { // ascending
        f = format::NONE;
        assert(f.eval(values, {10, 20, 30}) == score::GOLD, "Eval 1 (ASC)");
        assert(f.eval(values, {10, 30, 50}) == score::SILVER, "Eval 2 (ASC)");
        assert(f.eval(values, {10, 50, 90}) == score::BRONCE, "Eval 3 (ASC)");
        assert(f.eval(values, {50, 100, 150}) == score::NOTHING, "Eval 4 (ASC)");
    }
    { // descending
        f = format::MINUTES;
        assert(f.eval(values, {30, 20, 10}) == score::GOLD, "Eval 1 (DESC)");
        assert(f.eval(values, {15, 10, 5}) == score::SILVER, "Eval 2 (DESC)");
        assert(f.eval(values, {10, 5, 0}) == score::BRONCE, "Eval 3 (DESC)");
        assert(f.eval(values, {5, 3, 1}) == score::NOTHING, "Eval 4 (DESC)");
    }
    { // NONE
        f = format::NONE;
        f(1234, buf);
        assert(strcmp(buf, "1234"), "Value to String (NONE)");
    }
    { // METERS
        f = format::METERS;
        f(1234, buf);
        assert(strcmp(buf, "12,34"), "Value to String (METERS)");
    }
    { // MINUTES
        f = format::MINUTES;
        f(1234, buf);
        assert(strcmp(buf, "12:34"), "Value to String (MINUTES)");
    }
    { // SECONDS
        f = format::SECONDS;
        f(1234, buf);
        assert(strcmp(buf, "123,4"), "Value to String (SECONDS)");
    }
}

constexpr void discipline() {
    dsa::requirements_array req{{
        {{
            {{520, 440, 400}},
            {{510, 425, 345}},
            {{500, 420, 335}},
            {{450, 405, 325}},
            {{2050, 1850, 1650}},
            {{2020, 1820, 1620}}
        }},
        {{
            {{505, 420, 335}},
            {{445, 400, 315}},
            {{420, 340, 300}},
            {{405, 325, 245}},
            {{1750, 1550, 1350}},
            {{1720, 1520, 1320}}
        }}
    }};
    dsa::discipline disc{0, 0, dsa::format::MINUTES, 2, req};
    auto e = (disc.add(), disc.add());
    e.id() = 42069;
    e[0] = 500;
    assert(disc.end() - *disc.begin() == 6 && disc.end() - 3 == e, "Add");
    assert(e.id() == 42069 && e[0] == 500 && e[1] == static_cast<dsa::entry_type>(-1), "Entry");
    for(dsa::entry_type * ptr{*disc.begin()}; auto && entry : disc) assert(ptr == entry, "Iterator"), ptr += 3;
    assert(&dsa::levels({12, true}, disc.requirements) == &disc.requirements[1][1], "Levels");
}

int main() {
    try {
        io();
        formatter();
        discipline();
    } catch(char const * e) {
        std::cout << "Failed at: " << e << std::endl;
        return -1;
    }
}
