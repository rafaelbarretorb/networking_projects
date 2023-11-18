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
  echo_tcp_lib
  src/Acceptor.cpp
  src/Server.cpp
  src/Service.cpp
)

add_executable(async_tcp_server src/async_tcp_server.cpp)

target_link_libraries(
  async_tcp_server
  echo_tcp_lib
  ${Boost_LIBRARIES}
)

add_executable(async_tcp_client src/async_tcp_client.cpp)

target_link_libraries(
  async_tcp_client
  ${Boost_LIBRARIES}
)