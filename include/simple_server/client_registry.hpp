#pragma once
#include <memory>
#include <unordered_map>
#include <mutex>
#include <boost/core/noncopyable.hpp>

namespace simple_server {

class Client;

class ClientRegistry : boost::noncopyable {
public:
    bool Attach(const std::shared_ptr<Client> & client);
    void Detach(Client * client);
    void Stop();
private:
    using ClientMap = std::unordered_map<Client *, std::weak_ptr<Client>>;
    std::mutex mutex_;
    ClientMap clients_;
    bool stopped_{false};
};

} // namespace simple_server
