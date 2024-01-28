#include <simple_server/server.hpp>
#include <simple_server/hash_msg_handler.hpp>
#include <simple_server/thread_helper.hpp>
#include <boost/asio.hpp>

int main(int argc, char * argv[])
{
    boost::asio::io_context ioCtx;

    simple_server::Server server(ioCtx, std::make_shared<simple_server::HashHandlerContext>());

    boost::asio::signal_set signals(ioCtx, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) {
        server.Stop();
    });
    simple_server::ThreadPool workers(ioCtx);
    return 0;
}
