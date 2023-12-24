#include <fstream>
#include <nlohmann/json.hpp>
#include "jsonconfigreader.hpp"

namespace netlib {

using std::ifstream;

JsonConfigReader::JsonConfigReader(std::filesystem::path configPath)
{
    load(std::move(configPath));
}

bool JsonConfigReader::load(std::filesystem::path configPath)
{
    using namespace std::filesystem;
    if (exists(configPath)) {
        using json = nlohmann::json;

        ifstream f(configPath);
        try {
            json jsonConfig = json::parse(f);
            if (jsonConfig.contains("port"))
                m_nodePort  = jsonConfig["port"].get<std::uint16_t>();
        } catch (...) {
        }

    }

    return true;
}


const std::vector<LinkSettings> &JsonConfigReader::linkSettings() const
{
    return m_linkSettings;
}

std::optional<std::uint16_t> JsonConfigReader::nodePort() const
{
    return m_nodePort;
}

} // namespace netlib
