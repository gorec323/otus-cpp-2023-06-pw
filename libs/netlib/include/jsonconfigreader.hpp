#pragma once

#include <filesystem>
#include <vector>
#include "linksettings.hpp"

namespace netlib {

///
/// \brief The JsonConfigReader class Считыватель настроек из json файла
///
class JsonConfigReader
{
public:
    JsonConfigReader(std::filesystem::path configPath);

    bool load(std::filesystem::path configPath);

    const std::vector<LinkSettings> &linkSettings() const;

    std::optional<std::uint16_t> nodePort() const;

private:
    std::vector<LinkSettings> m_linkSettings;
    std::optional<std::uint16_t> m_nodePort;
 };

}
