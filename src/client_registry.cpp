#include <simple_server/client_registry.hpp>
#include <simple_server/client.hpp>

namespace simple_server {

bool ClientRegistry::Attach(const std::shared_ptr<Client> & client)
{
    std::lock_guard lock(mutex_);
    if (stopped_)
        return false;
    clients_[client.get()] = client;
    return true;
}

void ClientRegistry::Detach(Client * client)
{
    std::lock_guard lock(mutex_);
    clients_.erase(client);
}

void ClientRegistry::Stop()
{
    ClientMap clients;
    std::unique_lock lock(mutex_);
    stopped_ = true;
    clients.swap(clients_);
    lock.unlock();
    for (auto & [_, clientWeak] : clients) {
        if (auto client = clientWeak.lock()) {
            client->Stop();
        }
    }
}

} // namespace simple_server
