#ifndef RULES_RULES_HH_
# define RULES_RULES_HH_

# include <utils/dll.hh>

class Rules
{
public:
    explicit Rules(const std::string& champion);
    virtual ~Rules();

    // FIXME

private:
    utils::DLL* champion_dll_;

    // FIXME
};

#endif // !RULES_RULES_HH_
