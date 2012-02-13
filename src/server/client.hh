#ifndef SERVER_CLIENT_HH_
# define SERVER_CLIENT_HH_

class Client
{
public:
    Client(uint32_t id);

public:
    uint32_t id() const { return id_; }
    void id_set(uint32_t id) { id_ = id; }

private:
    uint32_t id_;
};

#endif // !SERVER_CLIENT_HH_
