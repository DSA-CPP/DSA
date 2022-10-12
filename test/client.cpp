#include "utils.hpp"

static void client() {
    using dsa::disciplines;
    auto invalid = static_cast<dsa::entry_type>(-1);
    dsa::client::context ctx{"Test-Station"};
    assert(ctx.name() == "Test-Station", "New Context");
    assert(!ctx.current(), "Empty Context");
    static_assert(dsa::name(disc) == "KraftWerfen");
    ctx.load(disc);
    assert(ctx.current(), "Non-Empty Context");
    auto r = ctx.entries();
    assert(*r.begin() == r.end(), "Empty Entries");
    auto e = (ctx.add(), ctx.add());
    e.id() = 42069;
    e[0] = 500;
    r = ctx.entries();
    assert(r.end() - *r.begin() == 8 && r.end() - 4 == e, "Add");
    auto e2 = ctx.entry_of(42069);
    assert(e == e2, "Entry of ID");
    assert(ctx.add().id() == invalid, "Empty Entry");
    e = ctx.entry_of(42069);
    r = ctx.entries();
    assert(r.end() - *r.begin() == 12 && r.end() - 8 == e, "Add 2");
    assert(e.id() == 42069 && e[0] == 500 && e[1] == invalid && e[2] == invalid, "Entry");
    // save & load
}

static void participants() {
    NET_INIT();
    dsa::client::context ctx{"Test-Station 2"};
    ctx.load_participants(ep);
    assert(ctx.participants().size() == parts.size(), "Load all");
    for(auto && [id, part] : parts) {
        auto && p = ctx.participants().at(id);
        assert(p.age == part.age && p.male == part.male, "Load correctly");
    }
}

static void values() {
    NET_INIT();
    dsa::client::context ctx{"Test-Station 3"};
    ctx.load(disc);
    for(std::size_t i{}; i < entries.size(); i += 4)
        std::memcpy(ctx.add(), &entries[i], 4 * sizeof(dsa::entry_type));
    ctx.send_values(ep);
}

static void test() {
    client();
    participants();
    values();
}
