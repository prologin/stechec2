#ifndef LIB_NET_SOCKET_HH_
# define LIB_NET_SOCKET_HH_

# include <string>
# include <zmq.hpp>

namespace net {

struct Message;

class Socket // abstract
{
public:
    Socket(const std::string& pubsub_addr,
           const std::string& reqrep_addr,
           int io_thread);
    virtual ~Socket();

    virtual void init() = 0;
    virtual bool send_msg(const Message& msg);
    virtual bool get_msg(Message** msg);

protected:
    std::string pubsub_addr_;
    std::string reqrep_addr_;

    zmq::context_t ctx_;
    zmq::socket_t* pubsub_sckt_;
    zmq::socket_t* reqrep_sckt_;
};

} // namespace net

#endif // !LIB_NETWORK_SOCKET_HH_
