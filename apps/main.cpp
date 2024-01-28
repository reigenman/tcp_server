#include <simple_server/server.hpp>
#include <simple_server/hash_msg_handler.hpp>
#include <simple_server/thread_helper.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

void RunServer(std::string host, std::string portStr)
{
    uint16_t port = 8088;
    if (host.empty()) {
        host = "127.0.0.1";
    }
    if (!portStr.empty()) {
        port = boost::lexical_cast<uint16_t>(portStr);
    }
    boost::asio::io_context ioCtx;

    simple_server::Server server(ioCtx, std::make_shared<simple_server::HashHandlerContext>(), host, port);

    boost::asio::signal_set signals(ioCtx, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) {
        server.Stop();
    });
    simple_server::ThreadPool workers(ioCtx);
}

int main(int argc, char * argv[])
{
    std::string host;
    std::string port;
    if (argc >= 2) {
        host = argv[1];
    }
    if (argc >= 3) {
        port = argv[2];
    }
    try {
        RunServer(host, port);
    } catch (const std::exception & ex) {
        std::cerr << "failed with exception: " << ex.what() << "\n";
    }

    return 0;
}
