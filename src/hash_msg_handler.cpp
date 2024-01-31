#include <simple_server/hash_msg_handler.hpp>
#include "openssl/sha.h"
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <array>
#include <boost/core/noncopyable.hpp>
#include <boost/core/span.hpp>

namespace simple_server {

namespace {

using MdCtxPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

template<typename Hash>
std::string HashToString(const Hash & hash)
{
    std::stringstream ss;
    for (auto b : hash) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(b);
    }
    return ss.str();
}

} // namespace

class HashStreamHandler::Private : boost::noncopyable {
public:
    Private()
        : md_(EVP_get_digestbyname("sha256"))
        , mdCtx_(EVP_MD_CTX_new(), EVP_MD_CTX_free)
    {
        if (!md_ || ! mdCtx_) {
            throw std::runtime_error("HashStreamHandler(): failed to create ctx");
        }
    }
    ~Private() = default;

    void HandleChunk(const std::string_view & chunk)
    {
        Init();
        if (!EVP_DigestUpdate(mdCtx_.get(), chunk.data(), chunk.size())) {
            throw std::runtime_error("HashStreamHandler: failed to handle chunk");
        }
    }

    std::string FinalizeMessage()
    {
        Init();
        inited_ = false;

        std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
        unsigned int hashSize = 0;
        if (!EVP_DigestFinal_ex(mdCtx_.get(), hash.data(), &hashSize)) {
            throw std::runtime_error("HashStreamHandler: failed to finalize msg");
        }

        return HashToString(boost::span<unsigned char>(hash).subspan(0, hashSize));
    }

    void Init()
    {
        if (inited_)
            return;
        if (!EVP_DigestInit_ex2(mdCtx_.get(), md_, nullptr)) {
            throw std::runtime_error("HashStreamHandler: failed to init context");
        }
        inited_ = true;
    }
private:
    // not owned, must not be deleted
    const EVP_MD * md_{nullptr};
    MdCtxPtr mdCtx_;
    bool inited_{false};
};


HashStreamHandler::HashStreamHandler()
    : p_(new Private())
{}

HashStreamHandler::~HashStreamHandler()
{}

void HashStreamHandler::HandleChunk(const std::string_view & chunk)
{
    p_->HandleChunk(chunk);
}

std::string HashStreamHandler::FinalizeMessage()
{
    return p_->FinalizeMessage();
}

std::string HashStreamHandler::HashMessage(const std::string_view & request)
{
    std::array<unsigned char, SHA256_DIGEST_LENGTH> hash;
    SHA256(reinterpret_cast<const unsigned char *>(request.data()), request.size(), hash.data());
    return HashToString(hash);
}

} // namespace simple_server
