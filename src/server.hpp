#ifndef _DSA_SERVER_HPP_
#define _DSA_SERVER_HPP_

#include <tuple>
#include <unordered_map>
#include <vector>
#include "NETpp/tcp.hpp"
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

    std::pair<discipline const *, std::vector<entry_type>> receive_values(net::tcp::connection const & conn) {
        auto ptr = [](auto & val) { return reinterpret_cast<char *>(&val); };
        auto recv_to = [&](auto & buf) { // TODO (may halt)
            conn.recvall({ptr(buf), sizeof(buf)});
        };
        count_type section, activity; // one byte -> no net-order
        recv_to(section);
        recv_to(activity);
        discipline const * disc{};
        for(auto && d : disciplines) {
            if(d.section != section || d.activity != activity)
                continue;
            disc = &d;
            break;
        }
        std::uint64_t size;
        recv_to(size);
        size = net::endian(size);
        std::vector<entry_type> entries;
        entries.resize(size);
        conn.recvall({ptr(entries[0]), size * sizeof(entry_type)}); // TODO (may halt aswell)
        return {disc, std::move(entries)};
    }

}

#endif
