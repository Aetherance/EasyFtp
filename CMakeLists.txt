# 根目录 CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(NetworkProject)

# 设置标准输出目录（关键修复）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # 动态库到 build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # 静态库到 build/lib

add_subdirectory(netlib)
add_subdirectory(client)
add_subdirectory(server)