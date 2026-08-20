cmake_minimum_required(VERSION 3.10)
project(mavlink-driver)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# find dependencies
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# find generated proto files
set(PROTO_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/connection.pb.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/datastream_service/datastream.pb.cc"
)
set(PROTO_HDRS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/connection.pb.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/datastream_service/datastream.pb.h"
)
set(GRPC_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/connection.grpc.pb.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/datastream_service/datastream.grpc.pb.cc"
)
set(GRPC_HDRS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/connection.grpc.pb.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/proto/datastream_service/datastream.grpc.pb.h"
)

add_executable(mavlink_driver
src/connection_service.cpp
src/connection_factory.cpp
src/connection_handler.cpp
src/datastream.cpp
src/datastream_factory.cpp
src/main.cpp
  ${PROTO_SRCS}
  ${GRPC_SRCS}
)

target_include_directories(mavlink_driver PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/proto
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mavlink_driver
  protobuf::libprotobuf
  gRPC::grpc
  gRPC::grpc++
  gRPC::grpc++_reflection
)
