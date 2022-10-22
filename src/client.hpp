#ifndef _DSA_CLIENT_HPP_
#define _DSA_CLIENT_HPP_

#include <filesystem>
#include <unordered_map>
#include "dsa.hpp"

namespace dsa::client {

    class context {
    public:
        template<typename T>
        explicit context(T && name) noexcept : station_name_{std::forward<T>(name)} {
            std::filesystem::create_directory(station_name_); // might throw
        }

        ~context() { if(disc_) save(); } // todo: save participants
        constexpr auto current() const noexcept { return disc_; }
        constexpr auto & name() const noexcept { return station_name_; }
        constexpr auto & participants() const noexcept { return parts_; }
        constexpr entry_range<entry_type      , int> entries()       noexcept { return {entries_, disc_->tries + 1}; }
        constexpr entry_range<entry_type const, int> entries() const noexcept { return {entries_, disc_->tries + 1}; }
        constexpr entry<entry_type> add() noexcept {
            auto size = entries_.size();
            entries_.resize(size + disc_->tries + 1, -1);
            return entries_.data() + size;
        }

        void save() const noexcept { io_.save(entries_); }
        void load(discipline const & disc) noexcept {
            if(disc_)
                save();
            disc_ = &disc;
            io_.filename = station_name_ + '/' + dsa::name(disc.id()) + ".dsa";
            io_.load(entries_);
        }

        void load_participants(net::tcp::connection const & conn) {
            parts_.clear();
            std::pair<entry_type, participant> buf;
            auto ptr = reinterpret_cast<char *>(&buf);
            while(conn.recv({ptr, sizeof(buf)}) == sizeof(buf)) {
                buf.first = net::endian(buf.first);
                buf.second.age = net::endian(buf.second.age);
                parts_.insert(buf);
            }
        }

        void send_values(net::tcp::connection const & conn) const {
            dsa::send_values(conn, disc_->id(), entries_);
        }

    private:
        std::unordered_map<entry_type, participant> parts_; // id - age, sex
        std::vector<entry_type> entries_;
        dsa::io<entry_type> io_;
        std::string station_name_;
        dsa::discipline const * disc_{};
    };

    class discipline_results {
    using iterator = std::vector<discipline const *>::iterator;
    using const_iterator = std::vector<discipline const *>::const_iterator;
    public:
        discipline_results(net::tcp::connection const & conn, entry_type id) {
            using namespace detail;
            conn.sendall(out(net::endian(id)));
            auto size = recv<std::uint64_t>(conn);
            discs_.reserve(size);
            while(size--) {
                try {
                    auto [id, data] = receive_values(conn, std::move(data_));
                    discs_.emplace_back(get_discipline(id)); // asserts exists
                    data_ = std::move(data);
                } catch(std::exception const &) {
                    break;
                }
            }
        }

        constexpr result_iterator<      iterator, entry_type      > begin()       noexcept { return {discs_.begin(), data_.data()}; }
        constexpr result_iterator<const_iterator, entry_type const> begin() const noexcept { return {discs_.begin(), data_.data()}; }
        constexpr auto end()       noexcept { return discs_.end(); }
        constexpr auto end() const noexcept { return discs_.end(); }
        constexpr auto size() const noexcept { return discs_.size(); }
    private:
        std::vector<discipline const *> discs_;
        std::vector<entry_type> data_;
    };

}

#endif // _DSA_CLIENT_HPP_
