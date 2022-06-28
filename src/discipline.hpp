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

    enum class format {
        NONE,    // ascending  - ^\d+$
        METERS,  // ascending  - ^\d+,\d\d$
        MINUTES, // descending - ^\d+:\d\d$
        SECONDS  // descending - ^\d+,\d$
    };

    struct entry {
    public:
        constexpr entry(types::entry_type const * entry) noexcept : ptr_{entry} {}
        constexpr auto & id() const noexcept { return *ptr_; }
        constexpr auto values() const noexcept { return ptr_ + 1; }
        constexpr auto & operator[](std::size_t pos) const noexcept { return values()[pos]; }
    private:
        types::entry_type const * const ptr_;
    };

    struct entry_iterator {
    public:
        constexpr entry_iterator(types::entry_type const * start, types::count_type tries) noexcept : ptr_{start}, tries_{tries} {}
        constexpr auto operator!=(types::entry_type const * sentinel) const noexcept { return ptr_ != sentinel; }
        constexpr auto & operator++() noexcept { return ptr_ += tries_, *this; }
        constexpr entry operator*() const noexcept { return ptr_; }
    private:
        types::entry_type const * ptr_;
        types::count_type const tries_;
    };

    class discipline {
    public:
        discipline(char const * name, types::count_type tries, format format/*, requirements*/); // LOAD!!!
        bool add(types::entry_type id) noexcept; // checks for existence and fills empty values with -1
        void change(types::entry_type const & ref, types::entry_type value); // SAVING!!!
        constexpr entry_iterator begin() const noexcept { return {entries_.data(), tries}; }
        constexpr types::entry_type const * end() const noexcept { return entries_.data() + entries_.size(); }
    private:
        // requirements
        std::vector<types::entry_type> entries_;
    public:
        char const * const name; // used for filename
        types::count_type const tries;
        format const format;
    };

    // TODO participants

}

#endif
