cmake_minimum_required(VERSION 3.19)
project(example-default)

set(SRCS main.cpp)

add_executable(example-default ${SRCS})

target_link_libraries(example-default MiniEngine)