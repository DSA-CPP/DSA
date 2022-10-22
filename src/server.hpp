#ifndef _DSA_SERVER_HPP_
#define _DSA_SERVER_HPP_

#include <filesystem>
#include <tuple>
#include <unordered_map>
#include "dsa.hpp"

namespace dsa::server {

    struct participant_data {}; // TODO add fields

    class context {
    using pair = std::pair<entry_type, participant>;
    using tuple = std::tuple<entry_type, participant, participant_data>;
    public:
        [[nodiscard]] std::vector<tuple> participants() const noexcept {
            std::vector<tuple> res;
            res.reserve(parts_.size());
            for(auto [id, part] : parts_) {
                id = net::endian(id);
                part.age = net::endian(part.age);
                res.emplace_back(id, part, data_.at(id));
            }
            return res;
        }

        bool emplace(entry_type id, participant part, participant_data const & data) noexcept {
            if(auto && [_, unique] = data_.emplace(id, data); !unique)
                return false;
            id = net::endian(id);
            part.age = net::endian(part.age);
            parts_.emplace_back(id, part);
            return true;
        }

        void send_participants(net::tcp::connection const & conn) const {
            conn.sendall({reinterpret_cast<char const *>(parts_.data()), parts_.size() * sizeof(pair)});
        }

    private:
        // TODO load
        std::unordered_map<entry_type, participant_data> data_;
        std::vector<pair> parts_; // network order
    };

    [[nodiscard]] constexpr io<entry_type> file(discipline_id id) noexcept {
        return {{static_cast<char>(id.section + '0'),
            static_cast<char>(id.activity + '0'),
            '.', 'd', 's', 'a'}}; // fuck u optimizer
    }

    // only valid names of known disciplines
    [[nodiscard]] inline std::vector<std::pair<discipline_id, io<entry_type>>> all_files(std::filesystem::path const & dir = ".") noexcept {
        std::vector<std::pair<discipline_id, io<entry_type>>> res;
        for(auto && f : std::filesystem::directory_iterator{dir}) {
            if(f.is_directory())
                continue;
            auto path = f.path().filename().string();
            if(path.length() != 6 || !path.ends_with(".dsa"))
                continue;
            discipline_id id{static_cast<count_type>(path[0] - '0'), static_cast<count_type>(path[1] - '0')};
            if(!get_discipline(id))
                continue;
            res.emplace_back(id, path);
        }
        return res;
    }

}

#endif
