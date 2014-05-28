#include "signal.hh"

#include <csignal>
#include <cstring>
#include <memory>
#include <zmq.hpp>
#include <utils/log.hh>
#include <utils/buffer.hh>
#include <net/message.hh>

namespace net {

static std::string addr_;
static int nb_client_;
static int client_id_;

static
void client_signal_handler(int sig)
{
    if (sig == SIGINT)
        INFO("Catched Ctrl-C, exiting cleanly...");
    else if (sig == SIGUSR1)
        INFO("Champion killed by sandbox, exiting cleanly...");
    else
        INFO("SIGSEGV, trying to exit cleanly...");

    zmq::context_t ctx(1);
    zmq::socket_t sckt(ctx, ZMQ_REQ);
    sckt.connect(addr_.c_str());

    utils::Buffer buf;
    Message msg(MSG_EXIT, client_id_);
    msg.handle_buffer(buf);

    sckt.send(buf.data(), buf.size());
}

static
void server_signal_handler(int sig)
{
    if (sig == SIGINT)
        INFO("Catched Ctrl-C, exiting cleanly...");
    else
        INFO("SIGSEGV, trying to exit cleanly...");

    zmq::context_t ctx(1);
    zmq::socket_t sckt(ctx, ZMQ_REP);
    sckt.bind(addr_.c_str());

    for (int i = 0; i < nb_client_; ++i)
    {
        zmq::message_t dummy_zmsg;
        sckt.recv(&dummy_zmsg);

        utils::Buffer buf;
        Message msg(MSG_EXIT);
        msg.handle_buffer(buf);

        sckt.send(buf.data(), buf.size());
    }
}

void handle_signals(signal_type stype, const std::string& addr, int nbr)
{
    // Set needed parameters in the interrupt handler
    addr_ = addr;

    if (stype == CLIENT_SIGNAL)
        client_id_ = nbr;
    else if (stype == SERVER_SIGNAL)
        nb_client_ = nbr;

    // Set interrupt handler
    struct sigaction act;

    if (sigemptyset(&act.sa_mask) == -1)
        WARN("Could not empty sigaction mask");

    int sigs = SIGINT | SIGSEGV;

    if (stype == CLIENT_SIGNAL)
    {
        sigs |= SIGUSR1;
        act.sa_handler = &client_signal_handler;
    }
    else if (stype == SERVER_SIGNAL)
        act.sa_handler = &server_signal_handler;

    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, nullptr) == -1)
        WARN("Could not place signal handler");
}

} // namespace net
