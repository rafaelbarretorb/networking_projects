cmake_minimum_required(VERSION 3.15)

project(asio)

set(CMAKE_CXX_STANDARD 17)

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
)

find_package(Boost 1.42.0 REQUIRED COMPONENTS system thread)

set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)

find_package(OpenGL REQUIRED)


if (UNIX)
    find_package(X11 REQUIRED)
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
endif (UNIX)

if (WIN32)
    include_directories(${WinSDK})
endif (WIN32)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


add_executable(MMO_Client src/MMO_Client.cpp)
target_link_libraries(
  MMO_Client
  ${Boost_LIBRARIES}
  Threads::Threads
  OpenGL::OpenGL
  OpenGL::GL
  OpenGL::GLX
)

add_executable(MMO_Server src/MMO_Server.cpp)
target_link_libraries(
  MMO_Server
  ${Boost_LIBRARIES}
  Threads::Threads
  OpenGL::OpenGL
  OpenGL::GL
  OpenGL::GLX
)

if (UNIX)
    target_link_libraries(MMO_Client
        ${X11_LIBRARIES}
        PNG::PNG
    )

    target_link_libraries(MMO_Server
        ${X11_LIBRARIES}
        PNG::PNG
    )
endif (UNIX)

# if (WIN32)
#     target_link_libraries(${Executable}
#         ${WinSDK}
#     )
# endif (WIN32)
