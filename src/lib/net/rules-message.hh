#ifndef RULES_MESSAGE_HH_
# define RULES_MESSAGE_HH_

# include <utils/buffer.hh>

// Rules have to implement this interface for their messages

class RulesMessage
{
public:
    // The RulesMessage in itself
    virtual void handle_buffer(utils::Buffer*) = 0;
};

#endif // !RULES_MESSAGE_HH_
