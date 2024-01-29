#include <simple_server/server.hpp>
#include <simple_server/hash_msg_handler.hpp>
#include <simple_server/thread_helper.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

constexpr uint16_t kDefaultPort = 8088;
constexpr std::string_view kDefaultHost = "0.0.0.0";

void RunServer(std::string host, uint16_t port)
{
    if (host.empty()) {
        host = kDefaultHost;
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
    uint16_t port = kDefaultPort;
    std::string host;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("addr", po::value<std::string>(&host)->default_value(std::string(kDefaultHost)), "tcp address to listen")
        ("port",  po::value<uint16_t>(&port)->default_value(kDefaultPort), "tcp port to listen in range 1-")
    ;

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }
    } catch (const std::exception & ex) {
        std::cerr << ex.what() << "\n";
        std::cerr << desc << "\n";
        return 2;
    }

    try {
        RunServer(host, port);
        return 0;
    } catch (const std::invalid_argument & ex) {
        std::cerr << "invalid program arguments: " << ex.what() << "\n";
    } catch (const std::exception & ex) {
        std::cerr << "failed with exception: " << ex.what() << "\n";
    }

    return 2;
}
