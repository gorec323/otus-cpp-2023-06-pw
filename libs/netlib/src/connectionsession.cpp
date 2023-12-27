#include <iostream>
#include <asio/buffer.hpp>
#include <asio/use_awaitable.hpp>
#include "connectionsession.hpp"

using asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;

namespace netlib {

ServerSideConnectionSession::ServerSideConnectionSession(tcp::socket socket):
    SocketConnectionSession(std::move(socket), true)
{
}

void ServerSideConnectionSession::start()
{
    auto ex = socket().get_executor();
    co_spawn(ex, [self = shared_from_base<ServerSideConnectionSession>()]{ return self->reader(); },
        asio::detached
    );

    co_spawn(ex, [self = shared_from_base<ServerSideConnectionSession>()]{ return self->writer(); },
        asio::detached
    );
}

awaitable<void> ServerSideConnectionSession::reader()
{
    try {
        for (std::string read_msg;;) {
            std::size_t n = co_await asio::async_read_until(socket(),
                                                            asio::dynamic_buffer(read_msg, 1024), "\n", use_awaitable);

            std::cout << "ServerSideConnectionSession::reader() " << n << std::endl;
            nootifyNewData(read_msg.substr(0, n));
            read_msg.erase(0, n);
        }
    } catch (std::exception& e) {
        std::cerr << "ConnectionSession::reader()" << e.what() << std::endl;
        stop();
    }
}

}
