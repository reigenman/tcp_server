#pragma once
#include <simple_server/net.hpp>
#include <simple_server/msg_handler.hpp>
#include <memory>
#include <boost/core/noncopyable.hpp>

namespace simple_server {

class ClientRegistry;

class Client : public std::enable_shared_from_this<Client>, boost::noncopyable {
public:
    Client(tcp::socket && socket, StreamHandlerPtr streamHandler, std::shared_ptr<ClientRegistry> registry);
    ~Client();

    void Start();
    void Stop();
private:
    void Read();
    void Write();
    void HandleRead(const bs::error_code & ec, size_t readNum);
    void HandleWrite(const bs::error_code & ec, size_t);
    void HandleStop();

    tcp::socket stream_;
    std::string logTag_;
    StreamHandlerPtr streamHandler_;
    std::string rdBuf_;
    std::string wrBuf_;
    bool stopped_{false};
    std::weak_ptr<ClientRegistry> registry_;
};

} // namespace simple_server
