#pragma once

#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <abstractconnectionsession.hpp>
#include <deque>

namespace netlib {

class SocketConnectionSession : public AbstractConnectionSession
{
public:
    SocketConnectionSession(asio::ip::tcp::socket socket, bool serverSide);

    bool serverSide() const  noexcept;

    virtual void start() = 0;

    void stop();


protected:
    asio::ip::tcp::socket &socket();
    asio::awaitable<void> writer();

private:

    asio::ip::tcp::socket m_socket;
    asio::steady_timer m_timer;
    std::deque<std::string> m_writeMsgs;
    const bool m_serverSide;

};

}
