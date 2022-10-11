#ifndef _DSA_CLIENT_HPP_
#define _DSA_CLIENT_HPP_

#include <unordered_map>
#include "NETpp/tcp.hpp"
#include "dsa.hpp"

namespace dsa::client {

    class context {
    public:
        template<typename T>
        constexpr context(T && name) noexcept : station_name_{std::forward<T>(name)} {}
        ~context() { if(disc_) save(); } // todo: save participants
        constexpr auto current() const noexcept { return disc_; }
        constexpr auto & name() const noexcept { return station_name_; }
        constexpr auto & participants() const noexcept { return parts_; }
        constexpr entry_range<entry_type      , int> entries()       noexcept { return {{entries_.data(), disc_->tries + 1}, entries_.end().base()}; }
        constexpr entry_range<entry_type const, int> entries() const noexcept { return {{entries_.data(), disc_->tries + 1}, entries_.end().base()}; }
        constexpr entry<entry_type      > entry_of(entry_type id)       noexcept { for(auto && e : entries()) if(e.id() == id) return e; return {}; }
        constexpr entry<entry_type const> entry_of(entry_type id) const noexcept { for(auto && e : entries()) if(e.id() == id) return e; return {}; }
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
            io_.filename = station_name_ + '/' + dsa::name(disc) + ".dsa";
            io_.load(entries_);
        }

        void load_participants(net::tcp::connection const & conn) noexcept {
            parts_.clear();
            std::pair<entry_type, participant> buf;
            auto ptr = reinterpret_cast<char *>(&buf);
            try {
                while(conn.recv({ptr, sizeof(buf)}) == sizeof(buf))
                    parts_.insert(buf);
            } catch(std::exception &) {}
        }

    private:
        std::unordered_map<entry_type, participant> parts_; // id - age, sex
        std::vector<entry_type> entries_;
        dsa::io<entry_type> io_;
        std::string station_name_;
        dsa::discipline const * disc_{};
    };

}

#endif // _DSA_CLIENT_HPP_
