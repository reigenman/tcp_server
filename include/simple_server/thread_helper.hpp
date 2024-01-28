#pragma once

#include <thread>
#include <boost/asio/io_context.hpp>

namespace simple_server {

class ThreadPool {
public:
    explicit ThreadPool(boost::asio::io_context & ioCtx, unsigned numThreads = 0);
    ~ThreadPool();

    ThreadPool(const ThreadPool & rhs) = delete;
    ThreadPool & operator=(const ThreadPool & rhs) = delete;

private:
    void Run();
    std::vector<std::thread> threads_;
};

} // namespace simple_server
