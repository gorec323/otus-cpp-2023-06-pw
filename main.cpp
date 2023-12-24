#include <iostream>
#include "version.h"
#include <networknode.hpp>

using namespace std;

int main(int argc, const char *argv[])
{
    using namespace std;

    cout << "Network node version: " << PROJECT_VERSION << endl;

    const auto paramsSize = argc;
    std::string configName;
    if (paramsSize == 2) {
        configName = argv[1];
    }

    netlib::NetworkNode networkNode;

    if (!configName.empty())
        networkNode.init(std::move(configName));

    return networkNode.run();
}
