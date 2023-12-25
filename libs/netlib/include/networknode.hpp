#pragma once

#include <string>
#include <memory>
#include <set>
#include "abstractconnectionsession.hpp"
#include <optional>
#include <vector>
#include "linksettings.hpp"

namespace netlib {

/// @brief Сетевой узел
class NetworkNode
{
public:
    void init(std::string configName = {});
    /// @brief Блокирующий вызов запуска узла
    /// @param port Порт
    /// @param threadsCount Количество потоков обработки
    /// @return 
    int run(std::size_t threadsCount = 1);

    using shared_connection_ptr = std::shared_ptr<AbstractConnectionSession>;
    ///
    /// \brief newConnection Обработчик нового соединения
    /// \param connection Новое соединение
    ///
    void newConnection(shared_connection_ptr connection);

    void disconnected(shared_connection_ptr connection);

private:
    void onConnectionsChanged();

    bool m_initialized {false};
    std::optional<std::uint16_t> m_serverPort;
    std::vector<LinkSettings> m_autoConnectionsSettings;
    std::set<shared_connection_ptr> m_connections;

};

}
