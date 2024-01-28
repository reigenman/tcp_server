#pragma once
#include <memory>
#include <string>

namespace simple_server {

class StreamHandler {
public:
    virtual ~StreamHandler() = default;

    virtual void HandleMessage(const std::string_view & request, std::string & response) = 0;
};

using StreamHandlerPtr = std::unique_ptr<StreamHandler>;

class HandlerContext {
public:
    virtual ~HandlerContext() = default;

    virtual StreamHandlerPtr CreateStreamHandler() = 0;
};

using HandlerContextPtr = std::shared_ptr<HandlerContext>;

} // namespace simple_server
