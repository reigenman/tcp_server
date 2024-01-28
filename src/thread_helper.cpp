#include <simple_server/thread_helper.hpp>
#include <iostream>
#include <boost/format.hpp>

namespace simple_server {

namespace {

void RunIoCtx(boost::asio::io_context & ioCtx)
{
    while (true) {
        try {
            ioCtx.run();
            return;
        } catch (const std::exception & ex) {
            std::cerr << boost::format("io.run() exception: %1%\n") % ex.what();
        }
    }
}

} // namespace

ThreadPool::ThreadPool(boost::asio::io_context & ioCtx, unsigned int numThreads)
{
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    numThreads = std::max(numThreads, 1u);
    for (unsigned i = 0; i < numThreads; ++i) {
        threads_.emplace_back([&context = ioCtx] {
            RunIoCtx(context);
        });
    }
    std::cerr << boost::format("run io.run() on %1% threads\n") % numThreads;
}

ThreadPool::~ThreadPool()
{
    for (auto & th : threads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

} // namespace simple_server
