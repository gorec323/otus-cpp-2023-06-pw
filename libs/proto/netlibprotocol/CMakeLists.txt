cmake_minimum_required(VERSION 3.8)

project(netlibprotocol  LANGUAGES CXX)

add_library(${PROJECT_NAME}
    include/crc.hpp
    include/netlibprotocol.hpp
    include/netlibheader.hpp
    src/netlibprotocol.cpp
    include/custompacket.hpp
    include/registrationrequest.hpp
    include/registrationresponse.hpp
    include/tunneldata.hpp
)

#if (${BUILD_TESTING})
#    add_subdirectory(tests)
#endif()

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(${PROJECT_NAME}
PRIVATE
    CRCPP_USE_CPP11
    CRCPP_USE_NAMESPACE
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    netlibprotobuflib
)
