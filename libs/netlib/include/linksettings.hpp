#pragma once

#include <optional>
#include <string>

namespace netlib {

///
/// \brief The LinkSettings class Структура для описания настройки соединения
///
struct LinkSettings
{
    std::string address; // удалённый адрес, для клиентского подключения
    std::optional<std::uint16_t> port; // порт
};

}
