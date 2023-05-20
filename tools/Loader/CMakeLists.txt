cmake_minimum_required(VERSION 3.19)
project(Loader)

#set(TARGET model-loader)

set(SRCS src/Loader.cpp)
set(HDRS src/Loader.h)

add_library(Loader STATIC ${HDRS} ${SRCS})

#install(TARGETS ${TARGET} RUNTIME DESTINATION bin)