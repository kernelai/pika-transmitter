cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/third/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(pika-transmister VERSION 0.1.0)

include(CTest)
enable_testing()

find_package (glog 0.5.0 REQUIRED)

add_executable(pika-transmister main.cpp)
target_link_libraries (pika-transmister glog::glog)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
