#include <iostream>
#include <asio/buffer.hpp>
#include <asio/use_awaitable.hpp>
#include "connectionsession.hpp"

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

namespace netlib {

ConnectionSession::ConnectionSession(tcp::socket socket):
    std::enable_shared_from_this<ConnectionSession>(),
    m_socket {std::move(socket)},
    m_timer(m_socket.get_executor())
{
}

awaitable<void> ConnectionSession::reader()
{
    try {
        for (std::string read_msg;;) {
            std::size_t n = co_await asio::async_read_until(m_socket,
                                                            asio::dynamic_buffer(read_msg, 1024), "\n", use_awaitable);
            // эхо
            m_write_msgs.emplace_back(read_msg.substr(0, n));
//            room_.deliver(read_msg.substr(0, n));
            read_msg.erase(0, n);
        }
    } catch (std::exception& e) {
        std::cerr << "ConnectionSession::reader()" << e.what() << std::endl;
        stop();
    }
}

asio::awaitable<void> ConnectionSession::writer()
{
    try {
        while (m_socket.is_open()) {
            if (m_write_msgs.empty()) {
                asio::error_code ec;
                co_await m_timer.async_wait(redirect_error(use_awaitable, ec));
            } else {
                co_await asio::async_write(m_socket,
                                           asio::buffer(m_write_msgs.front()), use_awaitable);
                if (m_write_msgs.front() == "10\n")
                    stop();
                m_write_msgs.pop_front();
            }
        }
    } catch (std::exception&) {
        stop();
    }
}

void ConnectionSession::start()
{
//    room_.join(shared_from_this());

    co_spawn(m_socket.get_executor(),
             [self = shared_from_this()]{ return self->reader(); },
    asio::detached);

    co_spawn(m_socket.get_executor(),
             [self = shared_from_this()]{ return self->writer(); },
    asio::detached);
}

void ConnectionSession::stop()
{
    //      room_.leave(shared_from_this());
    m_socket.close();
    m_timer.cancel();
}

}
