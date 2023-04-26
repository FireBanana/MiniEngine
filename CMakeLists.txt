cmake_minimum_required(VERSION 3.23.0)
project(MiniEngine)

include(FetchContent)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)
set(CMAKE_RUNTIME_LIBRARY_DIRECTORY bin/)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

add_definitions(-DGLFW_INCLUDE_NONE)
add_compile_definitions(DIR="${CMAKE_SOURCE_DIR}")
add_compile_definitions(USING_OPENGL)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(libs)
include_directories(libs/glad/include)
include_directories(libs/glm/glm)
include_directories(libs/glfw/include)

file( GLOB LIB_SOURCES 
	src/*.cpp
	src/components/*.cpp
	src/systems/*.cpp
	src/backend/*.cpp
	src/core/*.cpp
	src/core/utils/*.cpp
	src/core/types/*.cpp)

file( GLOB LIB_HEADERS 
	src/*.h
	src/components/*.h
	src/systems/*.h
	src/backend/*.h 
	src/core/*.h 
	src/core/utils/*.h
	src/core/types/*.h)

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

# Get additional libraries

FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp
	GIT_TAG v5.2.5
)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm
	GIT_TAG 0.9.9.8
)

OPTION (ASSIMP_BUILD_TESTS OFF)
OPTION( ASSIMP_INSTALL OFF)

FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
  FetchContent_Populate(assimp)
  add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
  add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_include_directories(MiniEngine PUBLIC ${assimp_SOURCE_DIR}/include)
target_include_directories(MiniEngine PUBLIC ${glm_SOURCE_DIR}/glm)
target_link_libraries(MiniEngine assimp)
target_link_libraries(MiniEngine glm)