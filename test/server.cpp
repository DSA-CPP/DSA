#include <cstring>
#include "utils.hpp"

static void participants(net::tcp::connection const & conn) {
    NET_INIT();
    dsa::server::context ctx;
    for(auto && [id, info] : parts)
        ctx.emplace(id, info, {});
    ctx.send_participants(conn);
}

static void values(net::tcp::connection const & conn) {
    NET_INIT();
    dsa::server::context ctx;
    auto [disc, entries] = dsa::server::receive_values(conn);
    assert(disc == &::disc &&
        entries.size() == ::entries.size() &&
        !std::memcmp(entries.data(), ::entries.data(), entries.size() * sizeof(dsa::entry_type)),
            "Receive entries");
}

static void test() {
    net::tcp::server serv{ep, 2};
    participants(serv);
    values(serv);
}
