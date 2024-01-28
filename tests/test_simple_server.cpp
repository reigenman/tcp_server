#include <gtest/gtest.h>
#include <simple_server/server.hpp>
#include <simple_server/hash_msg_handler.hpp>
#include <simple_server/thread_helper.hpp>
#include <boost/asio.hpp>

namespace net = boost::asio;
namespace bs = boost::system;
using tcp = net::ip::tcp;

namespace {
const std::string kHost = "127.0.0.1";
constexpr uint16_t kPort = 8088;

tcp::endpoint GetEndpoint()
{
    return tcp::endpoint(net::ip::make_address(kHost), kPort);
}

}

class ServerFixture : public testing::Test {
public:
    auto & GetIoContext()
    {
        return ioCtx_;
    }

    std::string GetResponse(const std::string & msg)
    {
        std::string result;
        hasher_.HandleMessage(std::string_view(msg).substr(0, msg.length() -1), result);
        result.push_back('\n');
        return result;
    }
protected:

    void SetUp() override
    {
        server_ = std::make_unique<simple_server::Server>(ioCtx_, std::make_shared<simple_server::HashHandlerContext>(), kHost, kPort);
        threads_ = std::make_unique<simple_server::ThreadPool>(ioCtx_, 1);
    }

    void TearDown() override
    {
    }
    boost::asio::io_context ioCtx_;
    std::unique_ptr<simple_server::ThreadPool> threads_;
    std::unique_ptr<simple_server::Server> server_;
    simple_server::HashStreamHandler hasher_;
};


TEST_F(ServerFixture, OneRequest)
{
    auto & ioCtx = GetIoContext();
    tcp::socket client(ioCtx);
    bs::error_code ec;
    client.connect(GetEndpoint(), ec);
    ASSERT_FALSE(ec);
    const std::string msg = "123\n";
    net::write(client, net::buffer(msg), ec);
    ASSERT_FALSE(ec);
    std::string response;
    auto len = net::read_until(client, net::dynamic_buffer(response), "\n", ec);
    ASSERT_FALSE(ec);
    ASSERT_EQ(response, GetResponse(msg));
    ASSERT_EQ(len, response.size());
}

TEST_F(ServerFixture, TwoRequests)
{
    auto & ioCtx = GetIoContext();
    tcp::socket client(ioCtx);
    bs::error_code ec;
    client.connect(GetEndpoint(), ec);
    ASSERT_FALSE(ec);
    const std::string msg1 = "123\n";
    const std::string msg2 = "456\n";
    auto msg = msg1 + msg2;
    net::write(client, net::buffer(msg), ec);
    ASSERT_FALSE(ec);
    std::string response;
    auto len = net::read_until(client, net::dynamic_buffer(response), "\n", ec);
    ASSERT_FALSE(ec);
    auto response1 = response.substr(0, len);
    ASSERT_EQ(response1, GetResponse(msg1));
    response.erase(0, len);

    len = net::read_until(client, net::dynamic_buffer(response), "\n", ec);
    ASSERT_FALSE(ec);
    auto response2 = response.substr(0, len);
    ASSERT_EQ(response2, GetResponse(msg2));
    response.erase(0, len);
    ASSERT_TRUE(response.empty());
}

TEST_F(ServerFixture, PartialRequests)
{
    auto & ioCtx = GetIoContext();
    tcp::socket client(ioCtx);
    bs::error_code ec;
    client.connect(GetEndpoint(), ec);
    ASSERT_FALSE(ec);
    const std::string msg = "123\n";
    for (char c : msg) {
        std::string part(1, c);
        net::write(client, net::buffer(part), ec);
        ASSERT_FALSE(ec);
    }
    std::string response;
    auto len = net::read_until(client, net::dynamic_buffer(response), "\n", ec);
    ASSERT_FALSE(ec);
    ASSERT_EQ(response, GetResponse(msg));
    ASSERT_EQ(len, response.size());
}

TEST_F(ServerFixture, TwoRequestsWithPartial)
{
    auto & ioCtx = GetIoContext();
    tcp::socket client(ioCtx);
    bs::error_code ec;
    client.connect(GetEndpoint(), ec);
    ASSERT_FALSE(ec);
    const std::string msg1 = "123\n";
    const std::string msg2 = "45689340983\n";
    size_t division = 4;
    auto msg = msg1 + msg2.substr(0, division);
    net::write(client, net::buffer(msg), ec);
    ASSERT_FALSE(ec);
    std::string response;
    auto len = net::read_until(client, net::dynamic_buffer(response), "\n", ec);
    ASSERT_FALSE(ec);
    auto response1 = response.substr(0, len);
    ASSERT_EQ(response1, GetResponse(msg1));

    // no partial responses
    response.clear();

    msg = msg2.substr(division);
    net::write(client, net::buffer(msg), ec);
    ASSERT_FALSE(ec);

    len = net::read_until(client, net::dynamic_buffer(response), "\n", ec);
    ASSERT_FALSE(ec);
    auto response2 = response.substr(0, len);
    ASSERT_EQ(response2, GetResponse(msg2));
    response.erase(0, len);
    ASSERT_TRUE(response.empty());
}

TEST_F(ServerFixture, MultipleClients)
{
    auto & ioCtx = GetIoContext();

    bs::error_code ec;
    std::vector<tcp::socket> clients;
    for (int i = 0; i < 10; ++i) {
        clients.emplace_back(ioCtx);
    }
    for (auto & client : clients) {
        client.connect(GetEndpoint(), ec);
        ASSERT_FALSE(ec);
    }
    std::vector<std::string> msgs;
    int i = 0;
    for (auto & client : clients) {
        ++i;
        std::string msg = std::to_string(i) + "123\n";
        net::write(client, net::buffer(msg), ec);
        ASSERT_FALSE(ec);
        msgs.push_back(msg);
    }
    for (size_t i = 0; i < clients.size(); ++i) {
        auto & client = clients[i];
        auto & msg = msgs.at(i);
        std::string response;
        auto len = net::read_until(client, net::dynamic_buffer(response), "\n", ec);
        ASSERT_FALSE(ec);
        ASSERT_EQ(response, GetResponse(msg));
        ASSERT_EQ(len, response.size());
    }
}
