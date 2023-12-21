#include <iostream>
#include <chrono>
#include <asio.hpp>
#include <set>
#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>
#include "networknode.hpp"
#include "connectionsession.hpp"

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

//awaitable<void> timeout(steady_clock::duration duration)
//{
//    asio::steady_timer timer(co_await this_coro::executor);
//    timer.expires_after(duration);
//    co_await timer.async_wait(use_nothrow_awaitable);
//}

//awaitable<void> transfer(tcp::socket& from, tcp::socket& to)
//{
//    std::array<char, 1024> data;

//    for (;;) {
//        const auto result1 = co_await (from.async_read_some(buffer(data), use_nothrow_awaitable)
//                                 || timeout(5s)
//                                 );

//        if (result1.index() == 1)
//            continue;
//        // co_return; // timed out

//        auto [e1, n1] = std::get<0>(result1);
//        if (e1)
//            break;

//        std::cout << std::string{data.data(), n1} << std::endl;
//        const auto result2 = co_await (
//                    // async_write(to, buffer(data, n1), use_nothrow_awaitable)
//                    async_write(from, buffer(data, n1), use_nothrow_awaitable)
//                    || timeout(1s)
//                    );

//        if (result2.index() == 1)
//            co_return; // timed out

//        auto [e2, n2] = std::get<0>(result2);
//        if (e2)
//            break;
//    }

//}

//awaitable<void> proxy(tcp::socket client)
//{
//    tcp::socket server(client.get_executor());

//    co_await transfer(client, server);
//}

awaitable<void> listen(tcp::acceptor& acceptor, NetworkNode &node)
{
    for (;;) {
        auto [e, client] = co_await acceptor.async_accept(use_nothrow_awaitable);
        if (e) {
            std::cerr << "listen" << e << std::endl;
            break;
        }

        std::cout << client.remote_endpoint() << std::endl;

        auto connection = std::make_shared<ConnectionSession>(std::move(client));
        node.newConnection(connection);
        connection->start();

//        auto ex = client.get_executor();
//        co_spawn(ex, proxy(std::move(client)), asio::detached);
    }
}

int NetworkNode::run(std::uint16_t port, std::size_t threaadCount)
{
    try {

        asio::io_context ctx;
        tcp::endpoint listen_endpoint = {tcp::v6(), port};

        // todo read settings client connections

        tcp::acceptor acceptor(ctx, listen_endpoint);

        co_spawn(ctx, listen(acceptor, *this), asio::detached);

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
