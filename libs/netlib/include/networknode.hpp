#pragma once

#include <string>
#include <memory>
#include <set>
#include "abstractnodeconnection.hpp"

namespace netlib {

/// @brief Сетевой узел
class NetworkNode
{
public:
    /// @brief Блокирующий вызов запуска узла
    /// @param port Порт
    /// @param threadsCount Количество потоков обработки
    /// @return 
    int run(std::uint16_t port = 9000, std::size_t threadsCount = 1);

    using shared_connection_ptr = std::shared_ptr<AbstractNodeConnection>;
    ///
    /// \brief newConnection Обработчик нового соединения
    /// \param connection Новое соединение
    ///
    void newConnection(shared_connection_ptr connection);

    void disconnected(shared_connection_ptr connection);

private:
    void onConnectionsChanged();

    std::set<shared_connection_ptr> m_connections;

};

}
