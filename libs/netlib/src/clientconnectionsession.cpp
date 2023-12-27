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

//using resolve_it_t = asio::ip::tcp::resolver::results_type::iterator;

//void start_connect(resolve_it_t begEndPointIter, resolve_it_t endEndPointIter)
//{
//    if (begEndPointIter != endEndPointIter) {
//        std::cout << "Trying " << begEndPointIter->endpoint() << "...\n";

//        // Set a deadline for the connect operation.
//        m_deadline.expires_after(std::chrono::seconds(60));

//        // Start the asynchronous connect operation.
//        using namespace std::placeholders;
//        socket().async_connect(endpoint_iter->endpoint(),
//                              std::bind(&ClientConnectionSession::handle_connect, this, _1, endpoint_iter));
//    }
//    else
//    {
//        // There are no more endpoints to try. Shut down the client.
//        stop();
//    }
//}

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
            const auto endPoints = co_await r.async_resolve(self->m_connectionSettings.address, std::to_string(self->m_connectionSettings.port.value()), use_awaitable);
            while (!m_stopped) {
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

//void ClientConnectionSession::start_connect(tcp::resolver::results_type::iterator endpoint_iter)
//{
//    if (endpoint_iter != m_endPoints.end()) {
//        std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

//        // Set a deadline for the connect operation.
//        m_deadline.expires_after(std::chrono::seconds(60));

//        // Start the asynchronous connect operation.
//        using namespace std::placeholders;
//        socket().async_connect(endpoint_iter->endpoint(),
//                              std::bind(&ClientConnectionSession::handle_connect, this, _1, endpoint_iter));
//    }
//    else
//    {
//        // There are no more endpoints to try. Shut down the client.
//        stop();
//    }
//}

//void ClientConnectionSession::handle_connect(const std::error_code &error, tcp::resolver::results_type::iterator endpoint_iter)
//{
//    if (m_stopped)
//        return;

//    // The async_connect() function automatically opens the socket at the start
//    // of the asynchronous operation. If the socket is closed at this time then
//    // the timeout handler must have run first.
//    if (!socket().is_open())
//    {
//        std::cout << "Connect timed out\n";

//        // Try the next available endpoint.
//        start_connect(++endpoint_iter);
//    }

//    // Check if the connect operation failed before the deadline expired.
//    else if (error)
//    {
//        std::cout << "Connect error: " << error.message() << "\n";

//        // We need to close the socket used in the previous connection attempt
//        // before starting a new one.
//        socket().close();

//        // Try the next available endpoint.
//        start_connect(++endpoint_iter);
//    }

//    // Otherwise we have successfully established a connection.
//    else
//    {
//        std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";

//        auto ex = socket().get_executor();
//        co_spawn(ex, [self = shared_from_base<ClientConnectionSession>()]{ return self->reader(); },
//        asio::detached
//        );

//        co_spawn(ex, [self = shared_from_base<ClientConnectionSession>()]{ return self->writer(); },
//        asio::detached
//        );    }
//}

void ClientConnectionSession::checkDeadline()
{
    if (m_stopped)
        return;

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (m_deadline.expiry() <= asio::steady_timer::clock_type::now()) {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        socket().close();

        // There is no longer an active deadline. The expiry is set to the
        // maximum time point so that the actor takes no action until a new
        // deadline is set.
        m_deadline.expires_at(asio::steady_timer::time_point::max());
    }

    // Put the actor back to sleep.
    m_deadline.async_wait(std::bind(&ClientConnectionSession::checkDeadline, this));
}

asio::awaitable<void> ClientConnectionSession::reader()
{
    try {
        for (std::string read_msg;;) {
            std::size_t n = co_await asio::async_read_until(socket(),
                                                            asio::dynamic_buffer(read_msg, 1024), "\n", use_awaitable);
            // эхо
            //            m_writeMsgs.emplace_back(read_msg.substr(0, n));
            //            room_.deliver(read_msg.substr(0, n));
            nootifyNewData(read_msg.substr(0, n));
            read_msg.erase(0, n);
        }
    } catch (std::exception& e) {
        std::cerr << "ConnectionSession::reader()" << e.what() << std::endl;
        stop();
    }
}

}
