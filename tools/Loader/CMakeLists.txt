cmake_minimum_required(VERSION 3.19)
project(Loader)

set(SRCS src/Loader.cpp)
set(HDRS src/Loader.h)

add_library(Loader STATIC ${HDRS} ${SRCS})

add_compile_options(-fPIC)

if(WIN32)
	target_link_libraries(Loader assimp)
endif()

target_include_directories(Loader PUBLIC ${CMAKE_SOURCE_DIR}/libs)
target_include_directories(Loader PUBLIC ${CMAKE_SOURCE_DIR}/libs/glad/include)
target_include_directories(Loader PUBLIC ${glm_SOURCE_DIR}/glm)
target_include_directories(Loader PUBLIC ${CMAKE_SOURCE_DIR}/libs/glfw/include)
target_include_directories(Loader PUBLIC ${assimp_SOURCE_DIR}/include)
target_include_directories(Loader PUBLIC ${assimp_BINARY_DIR}/include)


#install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
