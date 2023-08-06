cmake_minimum_required(VERSION 3.19)
project(Loader)

#set(TARGET model-loader)

set(SRCS src/Loader.cpp)
set(HDRS src/Loader.h)

include_directories(${glm_SOURCE_DIR}/glm)

add_library(Loader STATIC ${HDRS} ${SRCS})

#install(TARGETS ${TARGET} RUNTIME DESTINATION bin)