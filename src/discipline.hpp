#ifndef _DSA_DATA_HPP_
#define _DSA_DATA_HPP_

#include <cstdint>
#include <array>
#include <span>
#include <vector>
#include "utility.hpp"

namespace dsa {

    using entry_type = typename std::uint16_t;
    using count_type = typename std::uint16_t;
    using level_array = typename std::array<entry_type, 3>; // bronce, silver, gold
    using requirements_array = typename std::array<std::array<level_array, 6>, 2>; // 10-11, 12-13, 14-15, 16-17, 18-19, 20-24 | male, female

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

    class formatter {
    public:
        constexpr formatter(format format) noexcept : format_{format} {}
        entry_type value(char const * string) const noexcept;
        void display(entry_type value, char * buffer) const noexcept;
        score eval(std::span<entry_type> values) const noexcept;
    private:
        format const format_;
    };

    class discipline {
    public:
        discipline(char const * name, count_type tries, format format, requirements_array const & requirements); // loads from file
        level_array levels(bool male, std::uint8_t age) const; // throws on invalid age
        editor<entry_type> edit(entry<entry_type const> entry) noexcept; // saves
        constexpr entry_iterator<entry_type const, count_type> begin() const noexcept { return {entries_.data(), tries}; }
        constexpr auto end() const noexcept { return entries_.data() + entries_.size(); }
    private:
        std::vector<entry_type> entries_;
        requirements_array const & requirements_;
    public:
        char const * const name;
        count_type const tries;
        format const format;
    };

}

#endif
