cmake_minimum_required(VERSION 3.7)

project(netlibprotobuflib LANGUAGES CXX)

#include(ProtobufGenerateDfpost)

find_package(Protobuf REQUIRED) # NO_DEFAULT_PATH)

set(NETLIB_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)

file(GLOB NETLIB_PROTO_FILES
    ${NETLIB_PROTO_DIR}/connection.proto
)

# TODO: исправить все warning-и в файлах RDMP протокола
set(HIDE_PROTOC_WARNINGS TRUE)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${NETLIB_PROTO_FILES})

add_library(${PROJECT_NAME} STATIC ${PROTO_SRCS} ${PROTO_HDRS} ${NETLIB_PROTO_FILES})

target_compile_definitions(${PROJECT_NAME} PUBLIC PROTOBUF_USE_DLLS)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

#enable_msvc_system_include(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE
        ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    ${Protobuf_LITE_LIBRARIES}
)
