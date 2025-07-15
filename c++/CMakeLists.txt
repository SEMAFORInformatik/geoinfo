cmake_minimum_required(VERSION 3.15)

project(grpc_find_cities CXX)

set(CMAKE_CXX_STANDARD 20)  # valid values 11 14 17 20 24
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/common.cmake)

#find_package(Threads)

#find_package(Protobuf CONFIG REQUIRED)
#message(STATUS "Using protobuf ${Protobuf_VERSION}")

#set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

#find_package(gRPC CONFIG REQUIRED)
#message(STATUS "Using gRPC ${gRPC_VERSION}")

#set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

get_filename_component(gi_proto "../java/src/main/proto/geoinfo.proto" ABSOLUTE)
get_filename_component(gi_proto_path "${gi_proto}" PATH)

# Generated sources
set(gi_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/geoinfo.pb.cc")
set(gi_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/geoinfo.pb.h")
set(gi_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/geoinfo.grpc.pb.cc")
set(gi_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/geoinfo.grpc.pb.h")
add_custom_command(
      OUTPUT "${gi_proto_srcs}" "${gi_proto_hdrs}" "${gi_grpc_srcs}" "${gi_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${gi_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${gi_proto}"
      DEPENDS "${gi_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# gi_grpc_proto
add_library(gi_grpc_proto
  ${gi_grpc_srcs}
  ${gi_grpc_hdrs}
  ${gi_proto_srcs}
  ${gi_proto_hdrs})
target_link_libraries(gi_grpc_proto
  absl::check
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

#add_subdirectory(sample)

add_executable(grpc_server
        server/server.cpp)
##        server/find_cities.cpp)
target_link_libraries(grpc_server
    gi_grpc_proto
    absl::check
    absl::flags
    absl::flags_parse
    absl::log
    absl::log_initialize
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${Protobuf_LIBRARIES})

target_link_libraries(grpc_server m)
