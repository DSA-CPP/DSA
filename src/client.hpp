#ifndef _DSA_CLIENT_HPP_
#define _DSA_CLIENT_HPP_

#include <unordered_map>
#include "dsa.hpp"

namespace dsa::client {

    class context {
    public:
        context(std::string && name) noexcept : sname_{std::move(name)} {} // todo: load participants hence no constexpr
        ~context() { save(); } // todo: save participants
        constexpr auto current() const noexcept { return disc_; }
        constexpr auto & name() const noexcept { return sname_; }
        constexpr entry_range<entry_type      , int> entries()       noexcept { return {{entries_.data(), disc_->tries + 1}, entries_.end().base()}; }
        constexpr entry_range<entry_type const, int> entries() const noexcept { return {{entries_.data(), disc_->tries + 1}, entries_.end().base()}; }
        constexpr entry<entry_type      > entry_of(entry_type id)       noexcept { for(auto && e : entries()) if(e.id() == id) return e; return {}; }
        constexpr entry<entry_type const> entry_of(entry_type id) const noexcept { for(auto && e : entries()) if(e.id() == id) return e; return {}; }
        constexpr entry<entry_type> add() noexcept {
            auto size = entries_.size();
            entries_.resize(size + disc_->tries + 1, -1);
            return entries_.data() + size;
        }

        void save() const noexcept { if(disc_) io_.save(entries_); }
        void load(discipline const * disc) noexcept {
            save();
            disc_ = disc;
            io_.filename = sname_ + '/' + dsa::name(*disc) + ".dsa";
            io_.load(entries_);
        }

    private:
        std::unordered_map<entry_type, participant> parts_; // id - age, sex
        std::vector<entry_type> entries_;
        dsa::io<entry_type> io_;
        std::string sname_;
        dsa::discipline const * disc_{};
    };

}

#endif // _DSA_CLIENT_HPP_
