#pragma once

#include <linksettings.hpp>
#include <socketconnectionsession.hpp>

namespace netlib {

class ClientConnectionSession : public SocketConnectionSession
{
public:
    ClientConnectionSession(asio::ip::tcp::socket socket, const LinkSettings &connectionSettings);
    ~ClientConnectionSession();

    void start() override;

private:
    asio::awaitable<bool> connect(asio::ip::tcp::endpoint endPoint);

    asio::awaitable<void> reader();

    const LinkSettings m_connectionSettings;
    asio::steady_timer m_deadline;
    bool m_stopped {false};
};

}

