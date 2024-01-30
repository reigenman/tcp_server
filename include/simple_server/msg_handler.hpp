#pragma once
#include <memory>
#include <string>

namespace simple_server {

class StreamHandler {
public:
    virtual ~StreamHandler() = default;

    virtual void HandleChunk(const std::string_view & chunk) = 0;
    virtual std::string FinalizeMessge() = 0;
};

using StreamHandlerPtr = std::unique_ptr<StreamHandler>;

class HandlerContext {
public:
    virtual ~HandlerContext() = default;

    virtual StreamHandlerPtr CreateStreamHandler() = 0;
};

using HandlerContextPtr = std::shared_ptr<HandlerContext>;

} // namespace simple_server
