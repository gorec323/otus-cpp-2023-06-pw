#include <asio/use_awaitable.hpp>
#include "socketconnectionsession.hpp"

namespace netlib {

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

SocketConnectionSession::SocketConnectionSession(asio::ip::tcp::socket socket, bool serverSide):
    AbstractConnectionSession(),
//    std::enable_shared_from_this<ConnectionSession>(),
    m_socket {std::move(socket)},
    m_timer {m_socket.get_executor()},
    m_serverSide {serverSide}
{

}

bool SocketConnectionSession::serverSide() const noexcept
{
    return m_serverSide;
}



void SocketConnectionSession::stop()
{
    //      room_.leave(shared_from_this());
    m_socket.close();
    m_timer.cancel();
}

asio::awaitable<void> SocketConnectionSession::writer()
{
    try {
        while (m_socket.is_open()) {
            if (m_writeMsgs.empty()) {
                asio::error_code ec;
                co_await m_timer.async_wait(redirect_error(use_awaitable, ec));
            } else {
                co_await asio::async_write(m_socket,
                                           asio::buffer(m_writeMsgs.front()), use_awaitable);
                m_writeMsgs.pop_front();
            }
        }
    } catch (std::exception&) {
        stop();
    }
}



asio::ip::tcp::socket &SocketConnectionSession::socket()
{
    return m_socket;
}

}
