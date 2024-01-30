#pragma once
#include <simple_server/net.hpp>
#include <simple_server/msg_handler.hpp>
#include <simple_server/line_buf.hpp>
#include <memory>
#include <boost/core/noncopyable.hpp>

namespace simple_server {

class ClientRegistry;

class Client : public std::enable_shared_from_this<Client>, boost::noncopyable {
public:
    static constexpr size_t kDefaultReadBufSize = 2048;
    Client(tcp::socket && socket, StreamHandlerPtr streamHandler, std::shared_ptr<ClientRegistry> registry);
    ~Client();

    void Start();
    void Stop();
private:
    void UpdateDeadline();
    void Watchdog();
    void Read();
    void Write();
    void HandleRead(const bs::error_code & ec, size_t readNum);
    void HandleWrite(const bs::error_code & ec, size_t);
    void HandleChunk();
    void HandleStop();
    void CloseStream();
    void DoStop();

    tcp::socket stream_;
    std::chrono::steady_clock::time_point deadline_;
    net::steady_timer timer_;
    std::string logTag_;
    StreamHandlerPtr streamHandler_;

    // Size of the buffer for read depends on compromise beetween reading effecency of number sock.read() calls
    // and the memory consumption by concurrent clients.
    LineBuf rdBuf_{kDefaultReadBufSize};
    std::string wrBuf_;
    bool stopped_{false};
    std::weak_ptr<ClientRegistry> registry_;
};

} // namespace simple_server
