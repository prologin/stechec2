#ifndef LIB_NETWORK_SOCKET_HH_
# define LIB_NETWORK_SOCKET_HH_

# include <string>
# include <zmq.hpp>

namespace network {

class Message;

class Socket // abstract
{
public:
    Socket(const std::string& pubsub_addr,
           const std::string& reqrep_addr,
           int io_thread);
    virtual ~Socket();

    virtual void init() = 0;
    virtual void send_msg(const Message& msg);
    virtual void get_msg(Message* msg);

protected:
    std::string pubsub_addr_;
    std::string reqrep_addr_;

    zmq::context_t ctx_;
    zmq::socket_t* pubsub_sckt_;
    zmq::socket_t* reqrep_sckt_;
};

} // namespace network

#endif // !LIB_NETWORK_SOCKET_HH_
