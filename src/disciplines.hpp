#ifndef _DSA_DISCIPLINES_HPP_
#define _DSA_DISCIPLINES_HPP_

#include "dsa.hpp"

#define MAX_REQ {{static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1), static_cast<dsa::entry_type>(-1)}}
#define MIN_REQ {{0, 0, 0}}
#define POINT_REQ {{1, 2, 3}}

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
            : discipline{section, activity, format, tries, requirements}, io{} {
            char * dest = io.filename;
            for(auto src : {sections[section], activities[section][activity], ".dsa"})
                while(*src)
                    *dest++ = *src++;
            *dest = 0;
        }

    public:
        dsa::discipline discipline;
        dsa::io<entry_type, 32> io; // SchnelligkeitGeraetturnen & KoordinationSchleuderball - 25
    };

    inline constinit std::array<manager, 15> disciplines {{
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
        }}},
        {1, 2, format::METERS, 3, {{
            {{MAX_REQ, {{475, 525, 575}}, {{550, 600, 650}}, {{575, 625, 675}}, {{650, 700, 750}}, {{650, 700, 750}}}},
            {{MAX_REQ, {{625, 675, 725}}, {{700, 750, 800}}, {{750, 800, 850}}, {{775, 825, 875}}, {{775, 850, 900}}}}
        }}},
        {1, 4, format::METERS, 3, {{
            {{{{130, 145, 165}}, {{140, 160, 180}}, {{155, 170, 190}}, {{165, 180, 200}}, {{165, 185, 205}}, {{160, 180, 200}}}},
            {{{{150, 170, 185}}, {{170, 190, 205}}, {{190, 205, 225}}, {{205, 220, 240}}, {{210, 230, 250}}, {{210, 230, 250}}}}
        }}},
        {1, 5, format::NONE, 3, {{
            {{POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}},
            {{POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}}
        }}},
        {2, 0, format::SECONDS, 1, {{
            {{{{110, 101, 91}}, {{106, 96, 85}}, {{186, 170, 155}}, {{176, 163, 150}}, {{182, 165, 153}}, {{185, 168, 156}}}},
            {{{{103,  93, 84}}, {{ 97, 89, 81}}, {{170, 154, 141}}, {{163, 148, 135}}, {{160, 146, 132}}, {{158, 144, 130}}}}
        }}},
        {2, 1, format::SECONDS, 1, {{
            {{{{390, 315, 255}}, {{350, 280, 235}}, {{330, 275, 215}}, {{305, 255, 200}}, {{295, 240, 185}}, {{290, 240, 185}}}},
            {{{{360, 290, 225}}, {{330, 270, 210}}, {{310, 255, 200}}, {{295, 245, 190}}, {{280, 230, 175}}, {{270, 220, 155}}}}
        }}},
        {2, 3, format::NONE, 3, {{
            {{POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}},
            {{POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}}
        }}},
        {3, 0, format::METERS, 3, {{
            {{{{80, 90, 100}}, {{90, 100, 110}}, {{ 95, 105, 115}}, {{105, 115, 125}}, {{110, 120, 130}}, {{110, 120, 130}}}},
            {{{{85, 95, 105}}, {{95, 105, 115}}, {{110, 120, 130}}, {{120, 130, 140}}, {{130, 140, 150}}, {{135, 145, 155}}}},
        }}},
        {3, 1, format::METERS, 3, {{
            {{{{230, 260, 290}}, {{280, 310, 340}}, {{320, 350, 380}}, {{340, 370, 400}}, {{}}, {{}}}},
            {{{{260, 290, 320}}, {{320, 350, 380}}, {{380, 410, 440}}, {{430, 460, 490}}, {{}}, {{}}}}
        }}},
        {3, 4, format::METERS, 3, {{
            {{MAX_REQ, {{1700, 1950, 2200}}, {{1950, 2250, 2550}}, {{2200, 2500, 2800}}, {{2350, 2650, 2900}}, {{2400, 2700, 2950}}}},
            {{MAX_REQ, {{1950, 2400, 2750}}, {{2350, 2800, 3200}}, {{2750, 3200, 3650}}, {{3150, 3600, 4050}}, {{3300, 3800, 4200}}}}
        }}},
        {3, 5, format::NONE, 3, {{
            {{{{20, 30, 40}}, {{10, 20, 30}}, {{10, 15, 20}}, {{10, 15, 20}}, {{10, 20, 30}}, {{10, 20, 30}}}},
            {{{{20, 30, 40}}, {{10, 20, 30}}, {{10, 15, 20}}, {{10, 15, 20}}, {{5, 10, 15}}, {{ 5, 10, 15}}}}
        }}},
        {3, 6, format::NONE, 3, {{
            {{POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}},
            {{POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ, POINT_REQ}}
        }}}
    }};

}

#undef MAX_REQ
#undef MIN_REQ
#undef POINT_REQ

#endif
