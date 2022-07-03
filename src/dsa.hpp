#ifndef _DSA_DISCIPLINE_HPP_
#define _DSA_DISCIPLINE_HPP_

#include <cstdint>
#include <algorithm>
#include <array>
#include <unordered_map>
#include <vector>
#include "utility.hpp"

namespace dsa {

    using entry_type = typename std::uint16_t;
    using count_type = typename std::uint8_t;
    using level_array = typename std::array<entry_type, 3>; // bronce, silver, gold
    using requirements_array = typename std::array<std::array<level_array, 6>, 2>; // 10-11, 12-13, 14-15, 16-17, 18-19, 20-21 | male, female

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
        constexpr entry_type operator()(char const * string) const noexcept { return value<entry_type>(string).value_or(-1); }

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

        constexpr score eval(std::span<entry_type const> values, level_array const & levels) const noexcept {
            entry_type best;
            bool (*cmp)(entry_type, entry_type);
            switch(format_) {
            case format::NONE:
            case format::METERS:
                best = 0;
                cmp = [](entry_type a, entry_type b) { return a < b; };
                break;
            case format::MINUTES:
            case format::SECONDS:
                best = -1;
                cmp = [](entry_type a, entry_type b) { return a > b; };
                break;
            default: // format_ not recognized
                return score::INVALID;
            }
            for(auto e : values)
                if(e != static_cast<entry_type>(-1) && cmp(best, e))
                    best = e;
            return static_cast<score>(std::ranges::upper_bound(levels, best, cmp) - levels.begin());
        }
    private:
        format format_;
    };

    class discipline {
    public:
        constexpr discipline(count_type section, count_type activity, formatter formatter, count_type tries, requirements_array const & requirements, std::vector<entry_type> && entries = {}) noexcept
            : requirements{requirements}, entries_{std::move(entries)}, section{section}, activity{activity}, formatter{formatter}, tries{tries} {}
        constexpr count_type entry_size() const noexcept { return tries + 1; }
        constexpr entry<entry_type> add() noexcept { auto size = entries_.size(); entries_.resize(size + entry_size(), -1); return entries_.data() + size; }
        constexpr entry_iterator<entry_type,       count_type> begin()       noexcept { return {entries_.data(), entry_size()}; }
        constexpr entry_iterator<entry_type const, count_type> begin() const noexcept { return {entries_.data(), entry_size()}; }
        constexpr auto end()       noexcept { return entries_.data() + entries_.size(); }
        constexpr auto end() const noexcept { return entries_.data() + entries_.size(); }
        constexpr operator std::vector<entry_type> const &() const noexcept { return entries_; }
    public:
        requirements_array const requirements;
    private:
        std::vector<entry_type> entries_;
    public:
        count_type const section;
        count_type const activity;
        formatter const formatter;
        count_type const tries;
    };

    struct participant {
        std::uint8_t age;
        bool male;
    };

    constexpr auto & levels(participant part, requirements_array const & req) {
        return req[part.male].at(part.male);
    }

    //constexpr std::unordered_map<entry_type, participant const> map; // id - age, sex

}

#endif
