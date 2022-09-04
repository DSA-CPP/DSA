#ifndef _DSA_UTILITY_HPP_
#define _DSA_UTILITY_HPP_

#include <concepts>
#include <fstream>
#include <optional>
#include <span>
#include <string>
#include <vector>

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

    constexpr auto display(std::unsigned_integral auto value, char * buffer) noexcept {
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
        constexpr entry(T * first_element = nullptr) noexcept : ptr_{first_element} {}
        constexpr auto & id() const noexcept { return *ptr_; }
        constexpr auto values() const noexcept { return ptr_ + 1; }
        constexpr auto & operator[](std::size_t pos) const noexcept { return values()[pos]; }
        constexpr operator T *() const noexcept { return ptr_; }
    private:
        T * ptr_;
    };

    template<typename T, std::integral U>
    struct entry_iterator {
    public:
        constexpr entry_iterator(T * start, U stride) noexcept : ptr_{start}, stride_{stride} {}
        constexpr auto operator!=(T const * sentinel) const noexcept { return ptr_ != sentinel; }
        constexpr auto & operator++() noexcept { return ptr_ += stride_, *this; }
        constexpr entry<T> operator*() const noexcept { return ptr_; }
    private:
        T * ptr_;
        U const stride_;
    };

    template<typename T, std::integral U>
    struct entry_range {
        entry_iterator<T, U> begin_;
        T * end_;
        constexpr auto begin() const noexcept { return begin; }
        constexpr auto   end() const noexcept { return end; }
    };

    template<typename T>
    class io {
    public:
        void load(std::vector<T> & buffer) const noexcept {
            buffer.clear();
            std::ifstream file{filename, std::ios_base::binary};
            if(!file) return;
            std::uint32_t size;
            file.read(reinterpret_cast<char *>(&size), sizeof(size));
            buffer.resize(size);
            file.read(reinterpret_cast<char *>(buffer.data()), size * sizeof(T));
            if(!file) buffer.clear();
        }

        std::vector<T> load() const noexcept {
            std::vector<T> vec;
            load(vec);
            return vec;
        }

        void save(std::span<T const> span) const noexcept { // theoretical specialization for static extent?
            std::ofstream file{filename, std::ios_base::binary};
            std::uint32_t size{span.size()};
            file.write(reinterpret_cast<char const *>(&size), sizeof(size));
            file.write(reinterpret_cast<char const *>(span.data()), size * sizeof(T)); // not size_bytes() for symmetry
        }

    public:
        std::string filename;
    };

}

#endif
