#ifndef _DSA_UTILITY_HPP_
#define _DSA_UTILITY_HPP_

#include <concepts>
#include <fstream>
#include <iterator>
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

    constexpr char * display(std::unsigned_integral auto value, char * buffer) noexcept {
        char bp[20], * sp{bp};
        for(; value; value /= 10)
            *sp++ = static_cast<char>(value % 10) + '0';
        while(bp != sp)
            *buffer++ = *--sp;
        return buffer;
    }

    template<typename T>
    struct entry {
    public:
        constexpr entry(T * first_element) noexcept : ptr_{first_element} {}
        constexpr auto & id() const noexcept { return *ptr_; }
        constexpr auto values() const noexcept { return ptr_ + 1; }
        constexpr auto & operator[](std::size_t pos) const noexcept { return values()[pos]; }
        constexpr operator T *() const noexcept { return ptr_; }
    private:
        T * const ptr_;
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

    template<typename T>
    class io {
    public:
        constexpr io(char const * filename) : filename_{filename} {}

        std::vector<T> load() const noexcept {
            std::basic_ifstream<T> file{filename_, std::ios_base::binary};
            if(file.good()) return {std::istream_iterator{file}, {}};
            return {};
        }

        void save(entry<T> entry, std::streamsize tries) const noexcept {
            std::basic_ostream<T> file{filename_, std::ios_base::binary | std::ios_base::app};
            if(file.good()) file.write(entry, tries);
        }

        void save(std::vector<T> const & vec) const noexcept {
            std::basic_ostream<T> file{filename_, std::ios_base::binary};
            if(file.good()) file.write(vec.data(), vec.size());
        }

    private:
        char const * filename_;
    };

}

#endif
