#pragma once
#include <simple_server/msg_handler.hpp>
#include <string>

namespace simple_server {

class HashStreamHandler : public StreamHandler {
public:
    void HandleChunk(const std::string_view & chunk) override;
    std::string FinalizeMessge() override;

    void HandleMessage(const std::string_view & request, std::string & response);
private:
    std::string msg_;
};


class HashHandlerContext : public HandlerContext {
public:

    StreamHandlerPtr CreateStreamHandler() override
    {
        return std::make_unique<HashStreamHandler>();
    }
};

} // namespace simple_server
