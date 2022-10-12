#ifndef _DSA_SERVER_HPP_
#define _DSA_SERVER_HPP_

#include <tuple>
#include <unordered_map>
#include "dsa.hpp"

namespace dsa::server {

    struct participant_data {}; // TODO add fields

    class context {
    using pair = std::pair<entry_type, participant>;
    public:
        auto participants() const noexcept {
            std::vector<std::tuple<entry_type, participant, participant_data>> res;
            res.reserve(parts_.size());
            for(auto && [id, part] : parts_)
                res.emplace_back(net::endian(id), part, data_.at(id));
            return res;
        }

        bool emplace(entry_type id, participant part, participant_data const & data) noexcept {
            if(auto && [_, unique] = data_.emplace(id, data); !unique)
                return false;
            parts_.emplace_back(net::endian(id), part);
            return true;
        }

        void send_participants(net::tcp::connection const & conn) const noexcept {
            conn.sendall({reinterpret_cast<char const *>(parts_.data()), parts_.size() * sizeof(pair)});
        }

    private:
        // TODO load
        std::unordered_map<entry_type, participant_data> data_;
        std::vector<pair> parts_; // network order
    };

}

#endif
