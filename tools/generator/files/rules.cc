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
