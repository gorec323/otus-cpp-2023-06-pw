cmake_minimum_required(VERSION 3.8)

project(networklib  LANGUAGES CXX)

add_library(${PROJECT_NAME}
    include/networknode.hpp
    src/networknode.cpp
    include/route.hpp
    include/routetable.hpp
    src/route.cpp
    include/connectionsession.hpp
    src/connectionsession.cpp
    include/abstractconnectionsession.hpp
    src/abstractnodeconnection.cpp
    include/linksettings.hpp
    include/jsonconfigreader.hpp
    src/jsonconfigreader.cpp
    include/socketconnectionsession.hpp
    src/socketconnectionsession.cpp
    include/clientconnectionsession.hpp
    src/clientconnectionsession.cpp
)

#if (${BUILD_TESTING})
#    add_subdirectory(tests)
#endif()

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include

PRIVATE
    ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    nlohmann_json::nlohmann_json
)
