cmake_minimum_required(VERSION 3.23.0)
project(MiniEngine)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)
set(CMAKE_RUNTIME_LIBRARY_DIRECTORY bin/)

add_compile_definitions(DIR="${CMAKE_SOURCE_DIR}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(libs/glad/include)
include_directories(libs/glm/glm)
include_directories(libs/glfw/include)

file( GLOB LIB_SOURCES src/*.cpp src/core/*.cpp src/core/interfaces/*.cpp src/core/utils/*.cpp)
file( GLOB LIB_HEADERS src/*.h src/core/*.h src/core/interfaces/*.h src/core/utils/*.h)

add_executable( MiniEngine ${LIB_SOURCES} ${LIB_HEADERS} )
target_sources( MiniEngine PRIVATE ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c)

file( GLOB APP_SOURCES app/*.cxx )

foreach( testsourcefile ${APP_SOURCES} )
	string( REPLACE ".cpp" "" testname ${testsourcefile} )
	add_executable( ${testname} ${testsourcefile})	
	target_link_libraries( ${testname} MiniEngine )
endforeach( testsourcefile ${APP_SOURCES} )

target_link_libraries(MiniEngine ${CMAKE_SOURCE_DIR}/libs/glfw/lib-vc2022/glfw3.lib)
target_link_libraries(MiniEngine ${CMAKE_SOURCE_DIR}/libs/glfw/lib-vc2022/glfw3dll.lib)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_HEADERS})