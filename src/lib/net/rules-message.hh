#ifndef RULES_MESSAGE_HH_
# define RULES_MESSAGE_HH_

# include <cstdint>

// Rules have to implement this interface for their messages

class RulesMessage
{
public:
    // Size of the whole RulesMessage
    virtual uint32_t size() const = 0;
    virtual void size_set(uint32_t) = 0;

    // The RulesMessage in itself
    virtual void* data() = 0;
};

#endif // !RULES_MESSAGE_HH_
