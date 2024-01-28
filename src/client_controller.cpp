#include <simple_server/client_controller.hpp>
#include <simple_server/client.hpp>
#include <stdexcept>

namespace simple_server {

ClientController::ClientController(HandlerContextPtr handlerCtx)
    : handlerCtx_(std::move(handlerCtx))
    , registry_(std::make_shared<ClientRegistry>())
{
    if (!handlerCtx_) {
        throw std::invalid_argument("ClientController(): invalid handler");
    }
}

void ClientController::Accept(tcp::socket && socket)
{
    auto client = std::make_shared<Client>(std::move(socket), handlerCtx_->CreateStreamHandler(), registry_);
    client->Start();
}

void ClientController::Stop()
{
    registry_->Stop();
}

} // namespace simple_server
