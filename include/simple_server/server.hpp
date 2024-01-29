#pragma once
#include <memory>
#include <string>
#include <simple_server/msg_handler.hpp>
#include <boost/asio/io_context.hpp>
#include <stdexcept>

namespace simple_server {

class ServerInvalidArgError : public std::invalid_argument {
public:
    explicit ServerInvalidArgError(const std::string & what) : std::invalid_argument(what) {}
    explicit ServerInvalidArgError(const char* what) : std::invalid_argument(what) {}
};

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
