cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/third/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

set(Boost_NO_WARN_NEW_VERSIONS 1)

project(pika-transmister VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(wangle REQUIRED)
find_package(hiredis CONFIG REQUIRED)
add_executable(EchoClient src/example/EchoClient.cpp)
target_link_libraries(EchoClient PRIVATE wangle::wangle)
add_executable(EchoServer
 src/redis_decoder.cpp
 src/redis_decoder.h
 src/example/EchoServer.cpp)
target_link_libraries(EchoServer PRIVATE wangle::wangle)


find_package(GTest CONFIG REQUIRED)
add_executable(byteToRespDecoderTest src/redis_decoder.h)
target_link_libraries(byteToRespDecoderTest PRIVATE hiredis::hiredis wangle::wangle GTest::gmock_main)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
