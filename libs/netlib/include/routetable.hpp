#pragma once

#include <vector>
#include "route.hpp"

namespace netlib {

/// @brief Таблица маршрутизации
class RouteTable
{
public:
    RouteTable(std::initializer_list<Route> routes): m_routes {std::move(routes)} {}

private:
    std::vector<Route> m_routes;
};

}