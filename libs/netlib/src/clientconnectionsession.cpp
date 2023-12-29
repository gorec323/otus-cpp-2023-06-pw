#include <iostream>
#include <functional>
#include <chrono>

#include <asio/experimental/awaitable_operators.hpp>
#include <asio/experimental/as_tuple.hpp>
#include "clientconnectionsession.hpp"

namespace netlib {

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;
using std::chrono::steady_clock;

namespace this_coro = asio::this_coro;
using namespace asio::experimental::awaitable_operators;
using namespace std::literals::chrono_literals;

ClientConnectionSession::ClientConnectionSession(asio::ip::tcp::socket socket, const LinkSettings &connectionSettings):
    SocketConnectionSession(std::move(socket), false),
    m_connectionSettings {connectionSettings},
    m_deadline {this->socket().get_executor()}
{
}

ClientConnectionSession::~ClientConnectionSession()
{
    m_stopped = true;
    stop();
}

constexpr auto use_nothrow_awaitable = asio::experimental::as_tuple(asio::use_awaitable);

awaitable<void> timeout(steady_clock::duration duration)
{
    asio::steady_timer timer(co_await this_coro::executor);
    timer.expires_after(duration);
    co_await timer.async_wait(use_nothrow_awaitable);

}
asio::awaitable<bool> ClientConnectionSession::connect(asio::ip::tcp::endpoint endPoint)
{
    const auto res = co_await (socket().async_connect(endPoint, use_awaitable)
                                   || timeout(10s)
                                   );

    if (res.index() == 1) {
        socket().close();
         co_return false; // connect timed out
    }

    co_return socket().is_open();

}

void ClientConnectionSession::start()
{
    if (!m_connectionSettings.port.has_value())
        return;

    if (socket().is_open())
        return;

    auto ex = socket().get_executor();
    co_spawn(ex, [self = shared_from_base<ClientConnectionSession>(), this]() -> asio::awaitable<void>
        {
            auto ex = self->socket().get_executor();

            tcp::resolver r(ex);
            while (!m_stopped) {
                const auto endPoints = co_await r.async_resolve(self->m_connectionSettings.address, std::to_string(self->m_connectionSettings.port.value()), use_awaitable);
                bool connected {false};
                for (auto &&endPoint : endPoints) {
                    connected = co_await connect(endPoint);
                    if (connected) {
                        std::cout <<  "ClientConnectionSession::start() connected to " << endPoint.endpoint() << std::endl;
                        break;
                    }
                }

                if (m_stopped)
                    co_return;

                if (!connected) {
                    co_await timeout(10s); // ожидание для переподключения
                    continue;
                }

                co_spawn(ex, [self = shared_from_base<ClientConnectionSession>()]
                    {
                        return self->reader();
                    },
                    asio::detached
                );

                co_spawn(ex, [self = shared_from_base<ClientConnectionSession>()]
                    {
                        return self->writer();
                    },
                    asio::detached
                );

                co_await idle();

            }

        },
        asio::detached
    );

}

asio::awaitable<void> ClientConnectionSession::reader()
{
    try {
        for (std::string read_msg;;) {
            std::size_t n = co_await asio::async_read_until(socket(),
                                                            asio::dynamic_buffer(read_msg, 1024), "\n", use_awaitable);
            // эхо
            //            m_writeMsgs.emplace_back(read_msg.substr(0, n));
            nootifyNewData(read_msg.substr(0, n));
            read_msg.erase(0, n);
        }
    } catch (std::exception& e) {
        std::cerr << "ConnectionSession::reader()" << e.what() << std::endl;
        stop();
    }
}

}
