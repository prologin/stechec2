#include "rules.hh"

Rules::Rules(const std::string& champion)
{
    if (!champion.empty())
        champion_dll_ = new utils::DLL(champion);
}

Rules::~Rules()
{
    delete champion_dll_;
}
