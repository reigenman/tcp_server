#include <simple_server/server.hpp>
#include <simple_server/acceptor.hpp>
#include <simple_server/client_controller.hpp>
#include <atomic>

namespace simple_server {

class Server::Private {
public:
    Private(boost::asio::io_context & ioCtx, HandlerContextPtr && handler, const std::string & address, uint16_t port)
        : clientCtrl_(std::make_shared<ClientController>(std::move(handler)))
    {
        auto addr = net::ip::make_address(address);
        auto socketHandler = [clCtr = clientCtrl_](auto && sock) {
            clCtr->Accept(std::move(sock));
        };
        acceptor_ = std::make_shared<Acceptor>(ioCtx, tcp::endpoint(addr, port), std::move(socketHandler));
        acceptor_->Start();
    }
    ~Private()
    {
        Stop();
    }

    void Stop()
    {
        if (stopped_.exchange(true))
            return;
        acceptor_->Stop();
        clientCtrl_->Stop();
    }
private:
    std::atomic<bool> stopped_{false};
    std::shared_ptr<ClientController> clientCtrl_;
    std::shared_ptr<Acceptor> acceptor_;
};

Server::Server(boost::asio::io_context & ioCtx, HandlerContextPtr handler, const std::string & address, uint16_t port)
    : p_(new Private(ioCtx, std::move(handler), address, port))
{}

Server::~Server()
{}

void Server::Stop()
{
    p_->Stop();
}

} // namespace simple_server
