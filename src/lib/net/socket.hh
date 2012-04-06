#ifndef LIB_NET_SOCKET_HH_
# define LIB_NET_SOCKET_HH_

# include <string>
# include <memory>
# include <zmq.hpp>

namespace net {

struct Message;

// Socket is the common representation of the pair of ZeroMQ sockets Stechec
// needs to work. It is composed of the ZeroMQ context a PUB-SUB socket and
// a REQ-REP socket

class Socket // abstract
{
public:
    Socket(const std::string& pubsub_addr,
           const std::string& reqrep_addr,
           int io_thread);

    virtual void init() = 0;
    virtual bool send(const Message& msg);
    // get_msg allocates a Message, it has to be deleted after its use
    virtual Message* recv();

protected:
    std::string pubsub_addr_;
    std::string reqrep_addr_;

    zmq::context_t ctx_;
    std::unique_ptr<zmq::socket_t> pubsub_sckt_;
    std::unique_ptr<zmq::socket_t> reqrep_sckt_;
};

} // namespace net

#endif // !LIB_NETWORK_SOCKET_HH_
