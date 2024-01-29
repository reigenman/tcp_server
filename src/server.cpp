#include <simple_server/server.hpp>
#include <simple_server/acceptor.hpp>
#include <simple_server/client_controller.hpp>
#include <atomic>
#include <boost/format.hpp>

namespace simple_server {

class Server::Private {
public:
    Private(boost::asio::io_context & ioCtx, HandlerContextPtr && handler, const std::string & address, uint16_t port)
        : clientCtrl_(std::make_shared<ClientController>(std::move(handler)))
    {
        boost::system::error_code ec;
        auto addr = net::ip::make_address(address, ec);
        if (ec) {
            throw ServerInvalidArgError(boost::str(boost::format("Sever: tcp address %1% is invalid") % address));
        }
        if (port == 0) {
            throw ServerInvalidArgError(boost::str(boost::format("Sever: tcp port %1% is invalid") % port));
        }
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
