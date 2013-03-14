#include "rules.hh"

Rules::Rules(const rules::Options opt)
{
    if (!opt.champion_lib.empty())
        champion_dll_ = new utils::DLL(opt.champion_lib);

    // FIXME
}

Rules::~Rules()
{
    // FIXME

    delete champion_dll_;
}

void Rules::client_loop(rules::ClientMessenger_sptr msgr)
{
    // FIXME
}

void Rules::spectator_loop(rules::ClientMessenger_sptr msgr)
{
    // FIXME
}

void Rules::server_loop(rules::ServerMessenger_sptr msgr)
{
    // FIXME
}
