#ifndef _DSA_DISCIPLINES_HPP_
#define _DSA_DISCIPLINES_HPP_

#include "dsa.hpp"

#define MAX_REQ {{static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1)}}
#define MIN_REQ {{0, 0, 0}}

namespace dsa {

    inline constexpr std::array<char const *, 4> sections{{"Ausdauer", "Kraft", "Schnelligkeit", "Koordination"}};
    inline constexpr std::array<std::array<char const *, 7>, 4> activities{{
        {{"Laufen", 0, 0, 0, "Schwimmen"}},
        {{"Werfen", "Medizinball", "Kugelstoßen", 0, "Standweitsprung", "Geraetturnen"}},
        {{"Laufen", "Schwimmen", 0, "Geraetturnen"}},
        {{"Hochsprung", "Weitsprung", 0, 0, "Schleuderball", "Seilspringen", "Geraetturnen"}}
    }};

    struct manager {
    public:
        consteval manager(count_type section, count_type activity, formatter format, count_type tries, requirements_array requirements) noexcept
            : discipline{section, activity, format, tries, requirements} {
            char * dest = io.filename;
            auto concat = [&](char const * src) { while(*src) *dest++ = *src++; };
            concat(sections[section]);
            concat(activities[section][activity]);
            concat(".dsadata");
            *dest = 0;
        }

    public:
        discipline discipline;
        io<entry_type, 32> io{}; // SchnelligkeitGeraetturnen & KoordinationSchleuderball - 25
    };

    //inline constinit std::array<discipline, 15> disciplines[] {{
    inline constinit manager disciplines[] {
        {0, 0, format::MINUTES, 1, {{
            {{{{520, 440, 400}}, {{510, 425, 345}}, {{500, 420, 335}}, {{450, 405, 325}}, {{2050, 1850, 1650}}, {{2020, 1820, 1620}}}},
            {{{{505, 420, 335}}, {{445, 400, 315}}, {{420, 340, 300}}, {{405, 325, 245}}, {{1750, 1550, 1350}}, {{1720, 1520, 1320}}}}
        }}},
        {0, 4, format::MINUTES, 1, {{
            {{{{720, 625, 530}}, {{1450, 1255, 1100}}, {{1305, 1140, 1000}}, {{1150, 1030, 905}}, {{2400, 2110, 1825}}, {{2335, 2050, 1800}}}},
            {{{{700, 620, 510}}, {{1330, 1130,  945}}, {{1200, 1015,  850}}, {{1100,  940, 820}}, {{2400, 2110, 1825}}, {{2235, 1950, 1700}}}}
        }}},
        {1, 0, format::METERS, 3, {{
            {{{{1100, 1500, 1800}}, {{1500, 1800, 2200}}, {{2000, 2400, 2700}}, {{2400, 2700, 3100}}, MAX_REQ, MAX_REQ}},
            {{{{2100, 2500, 2800}}, {{2600, 3000, 3300}}, {{3000, 3400, 3700}}, {{3400, 3800, 4200}}, MAX_REQ, MAX_REQ}}
        }}},
        {1, 1, format::METERS, 3, {{
            {{MAX_REQ, MAX_REQ, MAX_REQ, MAX_REQ, {{ 750,  800,  875}}, {{ 750,  800,  875}}}},
            {{MAX_REQ, MAX_REQ, MAX_REQ, MAX_REQ, {{1025, 1100, 1175}}, {{1025, 1125, 1200}}}}
        }}}
    };
    //}};

}

#undef MAX_REQ
#undef MIN_REQ

#endif
