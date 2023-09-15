cmake_minimum_required(VERSION 3.19)
project(GlslCompiler)

set(SRCS src/GlslCompiler.cpp)
set(HDRS src/GlslCompiler.h)

add_library(GlslCompiler ${HDRS} ${SRCS})

target_include_directories(GlslCompiler PUBLIC ${spdlog_SOURCE_DIR}/include/)
target_include_directories(GlslCompiler PRIVATE ${CMAKE_SOURCE_DIR}/src/core/utils/)

target_link_libraries(GlslCompiler MiniEngine)

#install(TARGETS ${TARGET} RUNTIME DESTINATION bin)