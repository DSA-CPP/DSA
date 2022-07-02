#ifndef _DSA_DISCIPLINE_HPP_
#define _DSA_DISCIPLINE_HPP_

#include <cstdint>
#include <algorithm>
#include <array>
#include <span>
#include <vector>
#include "utility.hpp"

namespace dsa {

    using entry_type = typename std::uint16_t;
    using count_type = typename std::uint16_t;
    using level_array = typename std::array<entry_type, 3>; // bronce, silver, gold
    using requirements_array = typename std::array<std::array<level_array, 6>, 2>; // 10-11, 12-13, 14-15, 16-17, 18-19, 20-21 | male, female

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

    constexpr auto && levels(requirements_array const & requirements, bool male, std::uint8_t age) { return requirements[male].at(age / 2 - 5); }

    class formatter {
    public:
        constexpr formatter(format format) noexcept : format_{format} {}
        constexpr entry_type operator()(char const * string) const noexcept { return value<entry_type>(string).value_or(-1); }

        constexpr void operator()(entry_type value, char * buffer) const noexcept {
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
            *buffer = 0;
        }

        constexpr score eval(std::span<entry_type const> values, level_array const & levels) const noexcept {
            switch(format_) {
            case format::NONE:
            case format::METERS:
                return static_cast<score>(std::ranges::upper_bound(levels, *std::ranges::max_element(values))                 - levels.begin());
            case format::MINUTES:
            case format::SECONDS:
                return static_cast<score>(std::ranges::upper_bound(levels, *std::ranges::min_element(values), std::greater{}) - levels.begin());
            }
        }
    private:
        format const format_;
    };

    class discipline {
    public:
        constexpr discipline(char const * name, requirements_array const & requirements, count_type tries, format format, std::vector<entry_type> const & entries = {}) noexcept
            : entries_{entries}, name{name}, requirements{requirements}, tries{tries}, formatter{format} {}
        constexpr entry<entry_type> add() noexcept { auto size = entries_.size(); entries_.resize(size + tries, -1); return entries_.data() + size; }
        constexpr entry_iterator<entry_type,       count_type> begin()       noexcept { return {entries_.data(), tries}; }
        constexpr entry_iterator<entry_type const, count_type> begin() const noexcept { return {entries_.data(), tries}; }
        constexpr auto end()       noexcept { return entries_.data() + entries_.size(); }
        constexpr auto end() const noexcept { return entries_.data() + entries_.size(); }
        constexpr operator std::vector<entry_type> const &() const noexcept { return entries_; }
    private:
        std::vector<entry_type> entries_;
    public:
        char const * const name; // also filename (should be unique)
        requirements_array const & requirements;
        count_type const tries;
        formatter const formatter;
    };

}

#endif
