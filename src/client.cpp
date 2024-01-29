#include <simple_server/client.hpp>
#include <simple_server/client_registry.hpp>
#include <stdexcept>
#include <string_view>
#include <iostream>
#include <boost/format.hpp>

namespace simple_server {
using namespace std::chrono_literals;
namespace {

// telnet use \r\n
std::string_view DbgMsgStrip(std::string_view msg)
{
    if (msg.empty() || msg.back() != '\r')
        return msg;
    return msg.substr(0, msg.size() - 1);
}

} // namespace

Client::Client(tcp::socket && socket, StreamHandlerPtr streamHandler, std::shared_ptr<ClientRegistry> registry)
    : stream_(std::move(socket))
    , timer_(stream_.get_executor())
    , streamHandler_(std::move(streamHandler))
    , registry_(registry)
{
    if (!streamHandler_) {
        throw std::invalid_argument("Client(): invalid streamHandler");
    }
    auto remoteEp = stream_.remote_endpoint();
    logTag_ = boost::str(boost::format("client(%1%:%2%)") % remoteEp.address().to_string() % remoteEp.port());
    std::cerr << boost::format("Client() %1%\n") % logTag_;
}

Client::~Client()
{
    if (auto registry = registry_.lock()) {
        registry->Detach(this);
    }
    std::cerr << boost::format("~Client() %1%\n") % logTag_;
}

void Client::Start()
{
    auto self = shared_from_this();
    auto registry = registry_.lock();
    if (!registry || !registry->Attach(self)) {
        return;
    }
    UpdateDeadline();
    Read();
    net::post(stream_.get_executor(), [this, self]() {
        if (stopped_)
            return;
        Watchdog();
    });
}

void Client::Stop()
{
    auto self = shared_from_this();
    net::post(stream_.get_executor(), [this, self]() {
        HandleStop();
    });
}

void Client::UpdateDeadline()
{
    deadline_ = std::chrono::steady_clock::now() + 60s;
}

void Client::Watchdog()
{
    auto self = shared_from_this();
    timer_.expires_at(deadline_);
    timer_.async_wait([this, self](auto) {
        if (stopped_) {
            return;
        }
        auto now = std::chrono::steady_clock::now();
        if (deadline_ > now) {
            Watchdog();
            return;
        }

        stopped_ = true;
        CloseStream();
        std::cerr << boost::format("session %1% tout\n") % logTag_;
    });
}

void Client::Read()
{
    auto self = shared_from_this();
    net::async_read_until(stream_, net::dynamic_buffer(rdBuf_), "\n", [this, self](const auto & ec, auto n) {
        HandleRead(ec, n);
    });
}

void Client::Write()
{
    auto self = shared_from_this();
    net::async_write(stream_, net::buffer(wrBuf_), [this, self](const auto & ec, auto n) {
        HandleWrite(ec, n);
    });
}

void Client::HandleRead(const bs::error_code & ec, size_t readNum)
{
    if (stopped_)
        return;

    if (ec) {
        std::cerr << boost::format("session %1% read op finished with status: %2%\n") % logTag_ % ec.message();
        DoStop();
        return;
    }
    UpdateDeadline();
    // don't modify rdBuf while line is used
    const auto line = std::string_view(rdBuf_).substr(0, readNum - 1);
    std::cerr << boost::format("session %1% got message: \"%2%\", total buf size: %3%\n") % logTag_ % DbgMsgStrip(line) % rdBuf_.size();

    wrBuf_.clear();

    streamHandler_->HandleMessage(line, wrBuf_);

    rdBuf_.erase(0, readNum);

    wrBuf_.push_back('\n');
    Write();
}

void Client::HandleWrite(const bs::error_code & ec, size_t)
{
    if (stopped_)
        return;

    if (ec) {
        std::cerr << boost::format("session %1% write op finished with status: %2%\n") % logTag_ % ec.message();
        DoStop();
        return;
    }
    Read();
}

void Client::HandleStop()
{
    if (stopped_)
        return;
    DoStop();
}

void Client::CloseStream()
{
    bs::error_code ec;
    stream_.shutdown(tcp::socket::shutdown_both, ec);
    stream_.close(ec);
}

void Client::DoStop()
{
    stopped_ = true;

    bs::error_code ec;
    timer_.cancel(ec);
    CloseStream();
}

} // namespace simple_server
