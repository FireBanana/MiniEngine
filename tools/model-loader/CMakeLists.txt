cmake_minimum_required(VERSION 3.19)
project(model-loader)

set(TARGET model-loader)

set(SRCS src/model-loader.cpp)
set(HDRS src/model-loader.h)

add_library(${TARGET} STATIC ${HDRS} ${SRCS})

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)