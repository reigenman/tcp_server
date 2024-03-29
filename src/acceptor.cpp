#include <simple_server/acceptor.hpp>
#include <stdexcept>
#include <iostream>
#include <boost/format.hpp>

namespace simple_server {

Acceptor::Acceptor(net::io_context & ioContext, tcp::endpoint endpoint, ConnectionHandler && handler)
    : ioContext_(ioContext)
    , acceptor_(net::make_strand(ioContext), endpoint, true)
    , handler_(std::move(handler))
{
    if (!handler_) {
        throw std::invalid_argument("Acceptor(): invalid handler");
    }
    std::cerr << boost::format("acceptor listens an address: %1% and port %2%\n") % endpoint.address().to_string() % endpoint.port();
}

Acceptor::~Acceptor()
{
    std::cerr << "~Acceptor()\n";
}

void Acceptor::Start()
{
    Accept();
}

void Acceptor::Stop()
{
    auto self = shared_from_this();
    net::post(acceptor_.get_executor(), [this, self]() {
        HandleStop();
    });
}

void Acceptor::Accept()
{
    auto self = shared_from_this();

    acceptor_.async_accept(net::make_strand(ioContext_), [this, self](const auto & ec, auto socket) {
        if (!HandleAccept(ec, std::move(socket))) {
            return;
        }
        Accept();
    });
}

void Acceptor::HandleStop()
{
    if (stopped_)
        return;

    stopped_ = true;
    bs::error_code ec;
    acceptor_.close(ec);
}

bool Acceptor::HandleAccept(const bs::error_code & ec, tcp::socket && socket)
{
    if (stopped_)
        return false;

    if (ec) {
        std::cerr << boost::format("acceptor got an error: %1%\n") % ec.message();
    } else {
        try {
            handler_(std::move(socket));
        } catch (const std::exception & e) {
            std::cerr << boost::format("acceptor handler exception: %1%\n") % e.what();
        }

    }
    return true;
}

} // namespace simple_server
