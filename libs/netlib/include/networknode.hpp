#pragma once

#include <string>

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

};

}