cmake_minimum_required(VERSION 3.6)

project(networklib  LANGUAGES CXX)

add_library(${PROJECT_NAME}
    include/networknode.hpp
    src/networknode.cpp
    include/route.hpp
    include/routetable.hpp
    src/route.cpp

)

if (${BUILD_TESTING})
    add_subdirectory(tests)
endif()

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include

PRIVATE
    ${asio_SOURCE_DIR}/asio/include
)
