cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server 
    main.cc 
    server.cc
    ../proto/fileInfo.pb.cc
)

target_link_libraries(server 
    netlib
    protobuf
)

target_include_directories(server PRIVATE
    ../netlib
    ../proto
)

if(UNIX)
    target_link_libraries(server pthread)
endif()

# 删除原有的 set_target_properties 设置