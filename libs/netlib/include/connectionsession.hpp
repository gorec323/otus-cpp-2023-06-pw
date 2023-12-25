#pragma once

#include <deque>
#include "socketconnectionsession.hpp"

namespace netlib {

///
/// \brief The ConnectionSession class Сессия подключения
///
class ServerSideConnectionSession: public SocketConnectionSession
{
public:
    ServerSideConnectionSession(asio::ip::tcp::socket socket);

    void start() override;

private:
    asio::awaitable<void> reader();
};

}
