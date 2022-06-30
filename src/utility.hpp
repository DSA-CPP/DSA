#ifndef _DSA_UTILITY_HPP_
#define _DSA_UTILITY_HPP_

#include <concepts>
#include <optional>

namespace dsa {

    template<std::unsigned_integral T>
    constexpr std::optional<T> value(char const * string) noexcept {
        T accumulate{};
        for(char c = *string; *string; c = *++string) {
            if(c < '0' || c > '9') continue;
            T v{static_cast<T>(accumulate * 10 + c - '0')};
            if(v < accumulate) return {};
            accumulate = v;
        }
        return accumulate;
    }

    template<typename T>
    struct entry {
    public:
        constexpr entry(T * first_element) noexcept : ptr_{first_element} {}
        constexpr auto & id() const noexcept { return *ptr_; }
        constexpr auto values() const noexcept { return ptr_ + 1; }
        constexpr auto & operator[](std::size_t pos) const noexcept { return values()[pos]; }
    private:
        T * const ptr_;
    };

    template<typename T>
    struct editor : entry<T> {
    public:
        constexpr editor(T * first_element, void (* f)()) noexcept : entry<T>{first_element}, f_{f} {}
        constexpr ~editor() { f_(); }
    private:
        void (* const f_)();
    };

    template<typename T, std::integral U>
    struct entry_iterator {
    public:
        constexpr entry_iterator(T * start, U tries) noexcept : ptr_{start}, tries_{tries} {}
        constexpr auto operator!=(T const * sentinel) const noexcept { return ptr_ != sentinel; }
        constexpr auto & operator++() noexcept { return ptr_ += tries_, *this; }
        constexpr entry<T> operator*() const noexcept { return ptr_; }
    private:
        T * ptr_;
        U const tries_;
    };

}

#endif
