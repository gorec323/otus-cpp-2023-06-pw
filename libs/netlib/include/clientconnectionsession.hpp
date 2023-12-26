#pragma once

#include <linksettings.hpp>
#include <socketconnectionsession.hpp>

namespace netlib {

class ClientConnectionSession : public SocketConnectionSession
{
public:
    ClientConnectionSession(asio::ip::tcp::socket socket, const LinkSettings &connectionSettings);

    void start() override;

private:
//    void start_connect(asio::ip::tcp::resolver::results_type::iterator endpoint_iter);
    asio::awaitable<bool> connect(asio::ip::tcp::endpoint endPoint);
//    void handle_connect(const std::error_code& error,
//          asio::ip::tcp::resolver::results_type::iterator endpoint_iter);

    void checkDeadline();

    asio::awaitable<void> reader();

    const LinkSettings m_connectionSettings;
    asio::steady_timer m_deadline;
//    asio::ip::tcp::resolver::results_type m_endPoints;
    bool m_stopped {false};
};

}

