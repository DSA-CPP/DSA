#ifndef _DSA_DISCIPLINE_HPP_
#define _DSA_DISCIPLINE_HPP_

#include <cstdint>
#include <algorithm>
#include <array>
#include <ranges>
#include "utility.hpp"

#define MAX_REQ   {static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1)}
#define MIN_REQ   {0, 0, 0}
#define POINT_REQ {1, 2, 3}

namespace dsa {

    using entry_type = std::uint16_t;
    using count_type = std::uint8_t;
    using level_array = std::array<entry_type, 3>; // bronce, silver, gold
    using requirements_array = level_array[2][6]; // 10-11, 12-13, 14-15, 16-17, 18-19, 20-21 | male, female

    inline constexpr std::uint16_t port = 50'000;
    inline constexpr char const * service = "50000";

    enum class score : count_type {
        NOTHING,
        BRONCE,
        SILVER,
        GOLD,
        INVALID,
        UNKNOWN_ID
    };

    enum class format : count_type {
        NONE,    // ascending  - ^\d+$
        METERS,  // ascending  - ^\d+,\d\d$
        MINUTES, // descending - ^\d+:\d\d$
        SECONDS  // descending - ^\d+,\d$
    };

    class formatter {
    public:
        constexpr formatter(format format) noexcept : format_{format} {}
        constexpr auto operator()(char const * string) const noexcept { return value<entry_type>(string).value_or(-1); }

        constexpr void operator()(entry_type value, char * buffer) const noexcept {
            if(value != static_cast<entry_type>(-1)) {
                switch(format_) {
                case format::NONE:
                    buffer = display(value, buffer);
                    break;
                case format::METERS:
                    buffer = display(value / 100u, buffer);
                    *buffer++ = ',';
                    buffer = display(value % 100u, buffer);
                    break;
                case format::MINUTES:
                    buffer = display(value / 100u, buffer);
                    *buffer++ = ':';
                    buffer = display(value % 100u, buffer);
                    break;
                case format::SECONDS:
                    buffer = display(value / 10u, buffer);
                    *buffer++ = ',';
                    buffer = display(value % 10u, buffer);
                    break;
                }
            }
            *buffer = 0;
        }

        constexpr auto operator()(std::span<entry_type const> values, level_array const & levels) const noexcept {
            auto eval = [&](entry_type best, auto cmp) {
                auto valid = [](entry_type x) { return x != static_cast<entry_type>(-1); };
                auto better = [&](entry_type x) { return cmp(best, x); };
                for(auto e : values | std::views::filter(valid) | std::views::filter(better))
                    best = e;
                return static_cast<score>(std::ranges::find_if(levels, better) - levels.begin());
            };
            switch(format_) {
            case format::NONE:
            case format::METERS:
                return eval(0, std::less<entry_type>{});
            case format::MINUTES:
            case format::SECONDS:
                return eval(-1, std::greater<entry_type>{});
            default: // format_ not recognized
                return score::INVALID;
            }
        }

    private:
        dsa::format format_;
    };

    struct participant {
        std::uint8_t age;
        bool male;
        static constexpr bool valid_age(int age) noexcept { return 9 < age && age < 22; }
    };

    constexpr auto & levels_for(participant part, requirements_array const & req) noexcept { return req[part.male][part.age / 2 - 5]; }

    struct discipline {
        dsa::count_type section;
        dsa::count_type activity;
        dsa::formatter format; // effectively a set of non-static member functions
        dsa::count_type tries;
        dsa::requirements_array requirements;
        static constexpr char const * sections[4]{"Ausdauer", "Kraft", "Schnelligkeit", "Koordination"};
        static constexpr char const * activities[4][7]{
            {"Laufen", 0, 0, 0, "Schwimmen"},
            {"Werfen", "Medizinball", "Kugelstoßen", 0, "Standweitsprung", "Geraetturnen"},
            {"Laufen", "Schwimmen", 0, "Geraetturnen"},
            {"Hochsprung", "Weitsprung", 0, 0, "Schleuderball", "Seilspringen", "Geraetturnen"}
        };

    };

    constexpr std::string name(discipline const & disc) noexcept { return std::string{disc.sections[disc.section]} + disc.activities[disc.section][disc.activity]; }

    inline constexpr std::array<dsa::discipline, 15> disciplines{{
        {0, 0, format::MINUTES, 1, {
            {{520, 440, 400}, {510, 425, 345}, {500, 420, 335}, {450, 405, 325}, {2050, 1850, 1650}, {2020, 1820, 1620}},
            {{505, 420, 335}, {445, 400, 315}, {420, 340, 300}, {405, 325, 245}, {1750, 1550, 1350}, {1720, 1520, 1320}}
        }},
        {0, 4, format::MINUTES, 1, {
            {{720, 625, 530}, {1450, 1255, 1100}, {1305, 1140, 1000}, {1150, 1030, 905}, {2400, 2110, 1825}, {2335, 2050, 1800}},
            {{700, 620, 510}, {1330, 1130,  945}, {1200, 1015,  850}, {1100,  940, 820}, {2400, 2110, 1825}, {2235, 1950, 1700}}
        }},
        {1, 0, format::METERS, 3, {
            {{1100, 1500, 1800}, {1500, 1800, 2200}, {2000, 2400, 2700}, {2400, 2700, 3100}, MAX_REQ, MAX_REQ},
            {{2100, 2500, 2800}, {2600, 3000, 3300}, {3000, 3400, 3700}, {3400, 3800, 4200}, MAX_REQ, MAX_REQ}
        }},
        {1, 1, format::METERS, 3, {
            {MAX_REQ, MAX_REQ, MAX_REQ, MAX_REQ, { 750,  800,  875}, { 750,  800,  875}},
            {MAX_REQ, MAX_REQ, MAX_REQ, MAX_REQ, {1025, 1100, 1175}, {1025, 1125, 1200}}
        }},
        {1, 2, format::METERS, 3, {
            {MAX_REQ, {475, 525, 575}, {550, 600, 650}, {575, 625, 675}, {650, 700, 750}, {650, 700, 750}},
            {MAX_REQ, {625, 675, 725}, {700, 750, 800}, {750, 800, 850}, {775, 825, 875}, {775, 850, 900}}
        }},
        {1, 4, format::METERS, 3, {
            {{130, 145, 165}, {140, 160, 180}, {155, 170, 190}, {165, 180, 200}, {165, 185, 205}, {160, 180, 200}},
            {{150, 170, 185}, {170, 190, 205}, {190, 205, 225}, {205, 220, 240}, {210, 230, 250}, {210, 230, 250}}
        }},
        {1, 5, format::NONE, 3, {
            {POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ},
            {POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}
        }},
        {2, 0, format::SECONDS, 1, {
            {{110, 101, 91}, {106, 96, 85}, {186, 170, 155}, {176, 163, 150}, {182, 165, 153}, {185, 168, 156}},
            {{103,  93, 84}, { 97, 89, 81}, {170, 154, 141}, {163, 148, 135}, {160, 146, 132}, {158, 144, 130}}
        }},
        {2, 1, format::SECONDS, 1, {
            {{390, 315, 255}, {350, 280, 235}, {330, 275, 215}, {305, 255, 200}, {295, 240, 185}, {290, 240, 185}},
            {{360, 290, 225}, {330, 270, 210}, {310, 255, 200}, {295, 245, 190}, {280, 230, 175}, {270, 220, 155}}
        }},
        {2, 3, format::NONE, 3, {
            {POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ},
            {POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}
        }},
        {3, 0, format::METERS, 3, {
            {{80, 90, 100}, {90, 100, 110}, { 95, 105, 115}, {105, 115, 125}, {110, 120, 130}, {110, 120, 130}},
            {{85, 95, 105}, {95, 105, 115}, {110, 120, 130}, {120, 130, 140}, {130, 140, 150}, {135, 145, 155}},
        }},
        {3, 1, format::METERS, 3, {
            {{230, 260, 290}, {280, 310, 340}, {320, 350, 380}, {340, 370, 400}, {}, {}},
            {{260, 290, 320}, {320, 350, 380}, {380, 410, 440}, {430, 460, 490}, {}, {}}
        }},
        {3, 4, format::METERS, 3, {
            {MAX_REQ, {1700, 1950, 2200}, {1950, 2250, 2550}, {2200, 2500, 2800}, {2350, 2650, 2900}, {2400, 2700, 2950}},
            {MAX_REQ, {1950, 2400, 2750}, {2350, 2800, 3200}, {2750, 3200, 3650}, {3150, 3600, 4050}, {3300, 3800, 4200}}
        }},
        {3, 5, format::NONE, 3, {
            {{20, 30, 40}, {10, 20, 30}, {10, 15, 20}, {10, 15, 20}, {10, 20, 30}, {10, 20, 30}},
            {{20, 30, 40}, {10, 20, 30}, {10, 15, 20}, {10, 15, 20}, {5,  10, 15}, { 5, 10, 15}}
        }},
        {3, 6, format::NONE, 3, {
            {POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ},
            {POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}
        }}
    }};

    constexpr discipline const * get_discipline(count_type section, count_type activity) noexcept {
        for(auto && disc : disciplines)
            if(disc.section == section && disc.activity == activity)
                return &disc;
        return nullptr;
    }

}

#undef MAX_REQ
#undef MIN_REQ
#undef POINT_REQ

#endif
