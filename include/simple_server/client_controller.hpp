#pragma once
#include <simple_server/net.hpp>
#include <simple_server/client_registry.hpp>
#include <simple_server/msg_handler.hpp>

namespace simple_server {

class ClientController {
public:
    explicit ClientController(HandlerContextPtr handlerCtx);

    void Accept(tcp::socket && socket);
    void Stop();
private:
    HandlerContextPtr handlerCtx_;
    std::shared_ptr<ClientRegistry> registry_;
};



} // namespace simple_server
