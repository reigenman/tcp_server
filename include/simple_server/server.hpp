#pragma once
#include <memory>
#include <string>
#include <simple_server/msg_handler.hpp>
#include <boost/asio/io_context.hpp>

namespace simple_server {

class Server {
public:

    // throws exception if can't bind listening socket
    explicit Server(boost::asio::io_context & ioCtx, HandlerContextPtr handler, const std::string & address, uint16_t port);
    ~Server();

    Server(const Server &) = delete;
    Server & operator=(const Server &) = delete;
    Server(Server &&) = default;
    Server & operator=(Server &&) = default;

    void Stop();
private:
    class Private;
    std::unique_ptr<Private> p_;
};

} // namespace simple_server
