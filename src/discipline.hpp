#ifndef _DSA_DATA_HPP_
#define _DSA_DATA_HPP_

#include <array>
#include <vector>
#include "types.hpp"

namespace dsa {

    enum class score {
        INVALID,
        BRONCE,
        SILVER,
        GOLD,
        UNKNOWN_ID
    };

    struct entry {
    public:
        constexpr entry(types::entry_type const * entry) noexcept : ptr_{entry} {}
        constexpr auto id() const noexcept { return *ptr_; }
        constexpr auto values() const noexcept { return ptr_ + 1; }
        constexpr auto operator[](std::size_t pos) const noexcept { return values()[pos]; }
    protected:
        types::entry_type const * ptr_;
    };

    class discipline {
        using eval_func = score (*)(types::entry_type * values, std::array<types::entry_type, 3> requirements) noexcept; // cache scores outside
        using display_func = void (*)(types::entry_type value, char * buffer) noexcept; // only valid values + assuming buffer is big enough
        using convert_func = types::entry_type (*)(char const * value_string) noexcept;
    public:
        discipline(char const * name, types::count_type tries, eval_func eval, display_func display, convert_func convert); // LOAD!!!
        void add(types::entry_type id); // SAVING!!!
        void change_id(std::size_t pos, types::entry_type new_id); // SAVING!!!
        void change_value(std::size_t pos, types::count_type value_pos, char const * new_value); // SAVING!!!
        score eval(types::entry_type * values, types::age_type age, bool male) const noexcept;
        void display(types::entry_type value, char * buffer) const noexcept; // handle -1
        constexpr entry operator[](std::size_t pos) const noexcept { return entries_.data() + pos * (tries + 1); }
    private:
        std::vector<types::entry_type> entries_;
        eval_func eval_;
        display_func display_;
        convert_func convert_;
    public:
        char const * const name; // used for filename
        types::count_type const tries;
    };

    // TODO participants

}

#endif
