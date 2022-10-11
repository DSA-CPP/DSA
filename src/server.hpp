#ifndef _DSA_SERVER_HPP_
#define _DSA_SERVER_HPP_

#include <tuple>
#include <unordered_map>
#include <vector>
#include "NETpp/tcp.hpp"
#include "dsa.hpp"

namespace dsa::server {

    struct participant_data {}; // TODO add fields

    template<typename T, typename U, typename V, std::size_t Extent>
    inline std::vector<std::tuple<T, U, V>> merge(std::span<std::pair<T, U> const, Extent> vals, std::unordered_map<T, V> const & map) {
        std::vector<std::tuple<T, U, V>> res;
        res.reserve(vals.size());
        for(auto && [t, u] : vals)
            res.emplace_back(t, u, map.at(t));
        return res;
    }

    class context {
    using pair = std::pair<entry_type, participant>;
    public:
        context() noexcept {} // TODO load
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
        std::unordered_map<entry_type, participant_data> data_;
        std::vector<pair> parts_; // network order
    };

}

#endif
