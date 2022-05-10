cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/third/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

set(Boost_NO_WARN_NEW_VERSIONS 1)

project(pika-transmister VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(wangle REQUIRED)
add_executable(EchoClient EchoClient.cpp)
target_link_libraries(EchoClient PRIVATE wangle::wangle)
add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PRIVATE wangle::wangle)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
