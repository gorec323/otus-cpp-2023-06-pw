cmake_minimum_required(VERSION 3.14)

set(PATCH_VERSION 1 CACHE INTERNAL "Patch version. Default = 1")
set(PROJECT_VERSION 0.0.${PATCH_VERSION} CACHE INTERNAL "Application version" FORCE)

project(network_node VERSION ${PROJECT_VERSION} LANGUAGES CXX)

configure_file(
    version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0135)

  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# Нет CMakeLists.txt поэтому пришлось исопльзовать бустовую библиотеку
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG 814f67e # tag 1.29.0
)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
#    https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    GIT_TAG 9cca280 # tag v3.11.3
)

#FetchContent_Declare(
#    protobuf
#    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
#    GIT_TAG 7f94235 #v25.1
#    SOURCE_SUBDIR  cmake
#    FIND_PACKAGE_ARGS NAMES protobuf
#)

# FetchContent_Declare{
#     boostalign
#     GIT_REPOOSITORY https://github.com/boostorg/align.git
#     GIT_TAG master
# }

# FetchContent_Declare(
#     boostasio
#     GIT_REPOSITORY https://github.com/boostorg/asio.git
#     GIT_TAG 222ce92 # boost-1.84.0
# )

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(asio json)

if (BUILD_TESTING)
    include(CTest)
    enable_testing()
endif()

add_subdirectory(libs)

add_executable(${PROJECT_NAME}
    main.cpp
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${CMAKE_BINARY_DIR}
    ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    networklib
)

if (UNIX)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Network node application")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "My Corporation")
    include(CPack)
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen.conf.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf)

    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/README.md\ ${CMAKE_CURRENT_SOURCE_DIR}/ )

    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docs/)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})


    add_custom_target(doxygen COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        COMMENT "Generating docs"
        VERBATIM
    )
else(DOXYGEN_FOUND)
    message(STATUS "Doxygen not found - reference manual will not be created")
endif(DOXYGEN_FOUND)
