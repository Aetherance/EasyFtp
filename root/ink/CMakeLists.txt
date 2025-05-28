cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_executable(client 
    main.cc
    client.cc
    ../proto/fileInfo.pb.cc
)

target_link_libraries(client
    netlib
    protobuf
)

target_include_directories(client PRIVATE
    ../netlib
    ../proto
)

# 删除原有的 set_target_properties 设置