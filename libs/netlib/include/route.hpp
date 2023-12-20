#pragma once

#include <string>
#include <vector>

namespace netlib {

    /// @brief Класс хранения маршрута до сетевого узла
    class Route
    {
    public:
        /// @brief Конструктор
        /// @param nodeName Имя сетевого узла, для которого хранится маршрут
        Route(std::string nodeName);
    private:
        const std::string m_nodeName;
        std::vector<std::string> m_repeaterNodes;
    };
}