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

    using data_callback_t = std::function<void(const std::string &)>;

    void subscribe(data_callback_t handler);

protected:
    void nootifyNewData(const std::string &data);
    asio::ip::tcp::socket &socket();
    asio::awaitable<void> writer();
    asio::awaitable<void> idle();

private:

    asio::ip::tcp::socket m_socket;
    asio::steady_timer m_timer;
    std::deque<std::string> m_writeMsgs;
    const bool m_serverSide;
    std::vector<data_callback_t> m_callbacks;

};

}
