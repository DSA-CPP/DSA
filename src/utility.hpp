#ifndef _DSA_UTILITY_HPP_
#define _DSA_UTILITY_HPP_

#include <optional>
#include "types.hpp"

namespace dsa {

    constexpr std::optional<types::entry_type> id(char const * id_string) noexcept {
        int v{};
        while(*id_string)
            if(*id_string < '0' || *id_string > '9' || (v = v * 10 + *id_string++ - '0') >= 1 << sizeof(types::entry_type) * 8)
                return {};
        return v;
    }

}

#endif
