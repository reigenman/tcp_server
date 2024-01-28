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
        handler_(std::move(socket));
    }
    return true;
}

} // namespace simple_server
