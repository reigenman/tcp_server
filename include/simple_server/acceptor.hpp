#pragma once
#include <simple_server/net.hpp>
#include <memory>
#include <boost/core/noncopyable.hpp>

namespace simple_server {

using ConnectionHandler = std::function<void (tcp::socket &&)>;

class Acceptor : public std::enable_shared_from_this<Acceptor>, boost::noncopyable {
public:
    // throws exception if can't bind the endpoint
    Acceptor(net::io_context & ioContext, tcp::endpoint endpoint, ConnectionHandler && handler);
    ~Acceptor();
    void Start();
    void Stop();
private:
    void Accept();
    bool HandleAccept(const bs::error_code & ec, tcp::socket && socket);
    void HandleStop();

    net::io_context & ioContext_;
    tcp::acceptor acceptor_;
    ConnectionHandler handler_;
    bool stopped_{false};
};

} // namespace simple_server
