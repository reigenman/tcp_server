#pragma once
#include <simple_server/msg_handler.hpp>
#include <string>

namespace simple_server {

class HashStreamHandler : public StreamHandler {
public:
    HashStreamHandler();
    ~HashStreamHandler();

    HashStreamHandler(const HashStreamHandler &) = delete;
    HashStreamHandler & operator=(const HashStreamHandler &) = delete;
    HashStreamHandler(HashStreamHandler &&) = default;
    HashStreamHandler & operator=(HashStreamHandler &&) = default;


    void HandleChunk(const std::string_view & chunk) override;
    std::string FinalizeMessage() override;

    static std::string HashMessage(const std::string_view & request);
private:
    class Private;
    std::unique_ptr<Private> p_;
};


class HashHandlerContext : public HandlerContext {
public:

    StreamHandlerPtr CreateStreamHandler() override
    {
        return std::make_unique<HashStreamHandler>();
    }
};

} // namespace simple_server
