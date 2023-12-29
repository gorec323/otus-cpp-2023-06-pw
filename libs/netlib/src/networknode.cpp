#include <iostream>
#include <chrono>
#include <asio.hpp>
#include <set>
#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>
#include <jsonconfigreader.hpp>

#include "networknode.hpp"
#include "connectionsession.hpp"
#include "clientconnectionsession.hpp"

namespace netlib {

using asio::ip::tcp;
using asio::buffer;
using asio::co_spawn;
using asio::awaitable;
using std::chrono::steady_clock;
namespace this_coro = asio::this_coro;
using namespace asio::experimental::awaitable_operators;
using namespace std::literals::chrono_literals;

constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

awaitable<void> listen(tcp::acceptor& acceptor, NetworkNode &node)
{
    for (;;) {
        auto [e, client] = co_await acceptor.async_accept(use_nothrow_awaitable);
        if (e) {
            std::cerr << "listen" << e << std::endl;
            break;
        }

        std::cout << client.remote_endpoint() << std::endl;

        auto session = std::make_shared<ServerSideConnectionSession>(std::move(client));
        node.newConnection(session);
        session->start();
    }
}

awaitable<void> clients(asio::io_context &ctx, const std::vector<LinkSettings> &settings, NetworkNode &node)
{
    for (auto &&connectionSetting : settings) {

        auto session = std::make_shared<ClientConnectionSession>(tcp::socket{ctx}, connectionSetting);
//        node.newConnection(session);
        session->start();
    }

    co_return ;
}

void NetworkNode::init(std::string configName)
{
    if (m_initialized)
       return;

    auto configFileName = configName;
    if (configFileName.empty()) {
        static const auto DEFAULT_CONFIG_NAME  = "node_config";
        configFileName = DEFAULT_CONFIG_NAME;
    }

    configFileName.append(".json");
    JsonConfigReader cfgReaderr{configFileName};
    m_serverPort = cfgReaderr.nodePort();
    m_autoConnectionsSettings = cfgReaderr.linkSettings();

    m_initialized = true;
}

int NetworkNode::run(std::size_t threadsCount)
{
    try {
        init();
        asio::io_context ctx;
        const std::uint16_t DEFAULT_NODE_PORT  {9001};
        tcp::endpoint listen_endpoint = {tcp::v6(), m_serverPort.value_or(DEFAULT_NODE_PORT)};

        tcp::acceptor acceptor(ctx, listen_endpoint);

        co_spawn(ctx, listen(acceptor, *this), asio::detached);

        co_spawn(ctx, clients(ctx, m_autoConnectionsSettings, *this), asio::detached);

        ctx.run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

void NetworkNode::newConnection(shared_connection_ptr connection)
{
    const auto it = m_connections.insert(std::move(connection));
    if (it.second)
        onConnectionsChanged();
}

void NetworkNode::disconnected(shared_connection_ptr connection)
{
    if (m_connections.erase(connection) > 0)
        onConnectionsChanged();
}

void NetworkNode::onConnectionsChanged()
{
    std::cout << "NetworkNode::onConnectionsChanged()" << std::endl;
}

}
