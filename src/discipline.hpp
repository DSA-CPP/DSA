#ifndef _DSA_DATA_HPP_
#define _DSA_DATA_HPP_

#include <cstdint>
#include <array>
#include <vector>
#include "utility.hpp"

namespace dsa {

    enum class score {
        INVALID,
        BRONCE,
        SILVER,
        GOLD,
        UNKNOWN_ID
    };

    enum class format {
        NONE,    // ascending  - ^\d+$
        METERS,  // ascending  - ^\d+,\d\d$
        MINUTES, // descending - ^\d+:\d\d$
        SECONDS  // descending - ^\d+,\d$
    };

    class discipline {
    public:
        using type = typename std::uint16_t;
        using count = typename std::uint8_t;
        using level_array = typename std::array<type, 3>; // bronce, silver, gold
        using requirements_array = typename std::array<std::array<level_array, 3>, 2>; // 12-13, 14-15, 16-17 | male, female
    public:
        discipline(char const * name, count tries, format format, requirements_array const & requirements); // loads from file
        level_array levels(bool male, std::uint8_t age) const; // throws on invalid age
        editor<type> edit(entry<type const> entry) noexcept; // saves
        constexpr entry_iterator<type const, count> begin() const noexcept { return {entries_.data(), tries}; }
        constexpr auto end() const noexcept { return entries_.data() + entries_.size(); }
    private:
        std::vector<type> entries_;
        requirements_array const & requirements_;
    public:
        char const * const name;
        count const tries;
        format const format;
    };

}

#endif
