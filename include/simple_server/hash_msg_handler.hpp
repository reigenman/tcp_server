#pragma once
#include <simple_server/msg_handler.hpp>
#include <string>

namespace simple_server {

class HashStreamHandler : public StreamHandler {
public:
    void HandleMessage(const std::string_view & request, std::string & response) override
    {
        response = std::to_string(hasher_(request));
    }
private:
    std::hash<std::string_view> hasher_;
};


class HashHandlerContext : public HandlerContext {
public:

    StreamHandlerPtr CreateStreamHandler() override
    {
        return std::make_unique<HashStreamHandler>();
    }
};

} // namespace simple_server
