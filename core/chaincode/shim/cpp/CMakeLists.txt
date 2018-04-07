cmake_minimum_required (VERSION 3.0)
project (cppshim CXX)
find_package(Boost COMPONENTS log unit_test_framework REQUIRED)
find_package(Protobuf REQUIRED)

find_library(GRPC_LIBRARY NAMES grpc)
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin)

# Use latest C++ version
set(CMAKE_CXX_FLAGS "-std=c++17")

# TODO Cleanup call of protoc
set(PROTO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../protos)
set(Protobuf_IMPORT_DIRS ${PROTO_SOURCE_DIR})

file(GLOB PROTO_COMMON_FILES ${PROTO_SOURCE_DIR}/common/*.proto)
file(GLOB PROTO_MSP_FILES ${PROTO_SOURCE_DIR}/msp/*.proto)
file(GLOB PROTO_PEER_FILES ${PROTO_SOURCE_DIR}/peer/*.proto)

FILE(TO_NATIVE_PATH ${PROTO_SOURCE_DIR} PROTOMODEL_PATH)
FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/generated PROTOBINDING_PATH)

set(PROTO_SRCS)
set(PROTO_HDRS)

# Generate protobuf files
FOREACH(proto ${PROTO_COMMON_FILES} ${PROTO_MSP_FILES} ${PROTO_PEER_FILES})
    STRING(REPLACE "${PROTO_SOURCE_DIR}" "${PROTOBINDING_PATH}" CCFILE ${proto})
    STRING(REPLACE ".proto" ".pb.cc" CCFILE ${CCFILE})
    STRING(REPLACE ".pb.cc" ".pb.h" HFILE ${CCFILE})
    get_filename_component(ABS_PROTO ${proto} ABSOLUTE)

    FILE(TO_NATIVE_PATH ${proto} proto_native)

    list(APPEND PROTO_SRCS "${CCFILE}")
    list(APPEND PROTO_HDRS "${HFILE}")

   add_custom_command(
      OUTPUT "${CCFILE}"
             "${HFILE}"
      COMMAND  ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --proto_path=${PROTOMODEL_PATH} --cpp_out ${PROTOBINDING_PATH} ${proto_native}
      DEPENDS ${ABS_PROTO} ${Protobuf_PROTOC_EXECUTABLE}
      COMMENT "Running C++ protocol buffer compiler on ${proto}"
      VERBATIM )
ENDFOREACH(proto)

# Generate grpc service files
FOREACH(proto ${PROTO_COMMON_FILES} ${PROTO_MSP_FILES} ${PROTO_PEER_FILES})
    STRING(REPLACE "${PROTO_SOURCE_DIR}" "${PROTOBINDING_PATH}" CCFILE ${proto})
    STRING(REPLACE ".proto" ".grpc.pb.cc" CCFILE ${CCFILE})
    STRING(REPLACE ".pb.cc" ".pb.h" HFILE ${CCFILE})
    get_filename_component(ABS_PROTO ${proto} ABSOLUTE)

    FILE(TO_NATIVE_PATH ${proto} proto_native)

    list(APPEND PROTO_SRCS "${CCFILE}")
    list(APPEND PROTO_HDRS "${HFILE}")

   add_custom_command(
      OUTPUT "${CCFILE}"
             "${HFILE}"
      COMMAND  ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --proto_path=${PROTOMODEL_PATH}
           --grpc_out ${PROTOBINDING_PATH} ${proto_native}
           --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
      DEPENDS ${ABS_PROTO} ${Protobuf_PROTOC_EXECUTABLE}
      COMMENT "Running C++ grpc services compiler on ${proto}"
      VERBATIM )
ENDFOREACH(proto)
set_source_files_properties(${PROTO_HDRS} ${PROTO_SRCS} PROPERTIES GENERATED TRUE)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/generated)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/generated ${CMAKE_CURRENT_SOURCE_DIR}/include)

FILE(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

ADD_LIBRARY(cppshim ${PROTO_HDRS} ${PROTO_SRCS} ${SRCS} ${HDRS})
TARGET_LINK_LIBRARIES(cppshim ${PROTOBUF_LIBRARIES})

# Tests
enable_testing ()
add_subdirectory (test)

# Sample chaincode. Move to fabric-samples later.
add_subdirectory (sample)
