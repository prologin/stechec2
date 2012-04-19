#ifndef TICTACTOE_MESSAGE_HH_
# define TICTACTOE_MESSAGE_HH_

# include <net/rules-message.hh>

enum MessageType
{
    GRID        = 0,
    ACTION      = 1,
};

class Message : public net::RulesMessage
{
public:
    Message(MessageType type)
        : type_(static_cast<uint32_t>(type))
    {
    }

    MessageType type()
    {
        return static_cast<MessageType>(type_);
    }

    virtual void handle_buffer(utils::Buffer* buf)
    {
        buf->handle(type_);
    }

protected:
    uint32_t type_;
};

#endif // !TICTACTOE_MESSAGE_HH_
