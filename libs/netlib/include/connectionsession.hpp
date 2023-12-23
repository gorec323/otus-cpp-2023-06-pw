#pragma once

#include <memory>
#include <deque>
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include "abstractconnectionsession.hpp"

namespace netlib {

///
/// \brief The ConnectionSession class Сессия подключения
///
class ConnectionSession: public AbstractConnectionSession, public std::enable_shared_from_this<ConnectionSession>
{
public:
    ConnectionSession(asio::ip::tcp::socket socket);
    void start();

private:
    asio::awaitable<void> reader();

    asio::awaitable<void> writer();

    void stop();

    asio::ip::tcp::socket m_socket;
    asio::steady_timer m_timer;
    std::deque<std::string> m_writeMsgs;
};

}
