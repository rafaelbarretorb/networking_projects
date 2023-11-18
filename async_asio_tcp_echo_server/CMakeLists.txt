cmake_minimum_required(VERSION 3.12)

project(async_asio_tcp_echo_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

find_package(Boost 1.42.0 REQUIRED COMPONENTS system thread)

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
)

add_library(
  server_lib
  src/server/acceptor.cpp
  src/server/server.cpp
  src/server/service.cpp
)

add_executable(run_tcp_server src/run_tcp_server.cpp)

target_link_libraries(
  run_tcp_server
  server_lib
  ${Boost_LIBRARIES}
)

add_library(
  client_lib
  src/client/client.cpp
)

add_executable(run_tcp_client src/run_tcp_client.cpp)

target_link_libraries(
  run_tcp_client
  client_lib
  ${Boost_LIBRARIES}
)