#include <iostream>
#include "version.h"
#include <networknode.hpp>

using namespace std;

int main(int argc, const char *argv[])
{
    using namespace std;

    cout << "Network node version: " << PROJECT_VERSION << endl;

    // const auto paramsSize = argc;
    // if (paramsSize != 2) {
    //     std::cout << "Usage:" << std::endl;
    //     std::cout << argv[0] << " [node port number]" << std::endl;
    //     return 1;
    // }

    // обработка параметра командной строки с числом команд в блоке
    // const int bulkCommandsLimit = std::atoi(argv[1]);
    // if (bulkCommandsLimit < 1)
    //     return 0;

    netlib::NetworkNode networkNode;

    return networkNode.run();
}
