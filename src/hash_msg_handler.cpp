#include <simple_server/hash_msg_handler.hpp>
#include "openssl/sha.h"
#include <sstream>
#include <iomanip>
#include <array>

namespace simple_server {


void HashStreamHandler::HandleChunk(const std::string_view & chunk)
{
    msg_.append(chunk.begin(), chunk.end());
}

std::string HashStreamHandler::FinalizeMessage()
{
    std::string result;
    HandleMessage(msg_, result);
    msg_.clear();
    return result;
}

void HashStreamHandler::HandleMessage(const std::string_view & request, std::string & response)
{
    std::array<unsigned char, SHA256_DIGEST_LENGTH> hash;
    SHA256(reinterpret_cast<const unsigned char *>(request.data()), request.size(), hash.data());
    std::stringstream ss;
    for (auto b : hash) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(b);
    }
    response = ss.str();
}

} // namespace simple_server
