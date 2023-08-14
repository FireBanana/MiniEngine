cmake_minimum_required(VERSION 3.19)
project(Loader)

set(SRCS src/Loader.cpp)
set(HDRS src/Loader.h)

add_library(Loader ${HDRS} ${SRCS})

target_include_directories(Loader PUBLIC ${CMAKE_SOURCE_DIR}/libs)
target_include_directories(Loader PUBLIC ${CMAKE_SOURCE_DIR}/libs/glad/include)
target_include_directories(Loader PUBLIC ${glm_SOURCE_DIR}/glm)
target_include_directories(Loader PUBLIC ${CMAKE_SOURCE_DIR}/libs/glfw/include)

#install(TARGETS ${TARGET} RUNTIME DESTINATION bin)