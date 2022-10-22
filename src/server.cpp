#include "server.hpp"

static void values_in(net::tcp::connection const & conn) {
    auto [id, vals] = dsa::receive_values(conn);
    dsa::server::file(id).save(vals);
}

static void values_out(net::tcp::connection const & conn) {
    dsa::server::participant_values{dsa::detail::recv<dsa::entry_type>(conn)}.send(conn);
}

int main() {}
