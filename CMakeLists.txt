cmake_minimum_required(VERSION 3.23.0)
project(MiniEngine)

include(FetchContent)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)
set(CMAKE_RUNTIME_LIBRARY_DIRECTORY bin/)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
set(CMAKE_CXX_STANDARD 17)

add_definitions(-DGLFW_INCLUDE_NONE)
add_compile_definitions(DIR="${CMAKE_SOURCE_DIR}")
add_compile_definitions(USING_OPENGL)

#add_compile_definitions(TINYGLTF_IMPLEMENTATION)
#add_compile_definitions(STB_IMAGE_IMPLEMENTATION)
#add_compile_definitions(STB_IMAGE_WRITE_IMPLEMENTATION)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(SYSTEM libs)
include_directories(SYSTEM libs/glad/include)
include_directories(SYSTEM libs/glm/glm)
include_directories(SYSTEM libs/glfw/include)

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

target_include_directories(MiniEngine PRIVATE src/)
target_include_directories(MiniEngine PRIVATE src/components)
target_include_directories(MiniEngine PRIVATE src/systems)
target_include_directories(MiniEngine PRIVATE src/backend)
target_include_directories(MiniEngine PRIVATE src/core)
target_include_directories(MiniEngine PRIVATE src/core/utils)
target_include_directories(MiniEngine PRIVATE src/core/types)

if(MSVC)
  target_compile_options(MiniEngine PRIVATE /W4 /external:anglebrackets)
else()
  target_compile_options(MiniEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()

file( GLOB APP_SOURCES app/*.cxx )

foreach( testsourcefile ${APP_SOURCES} )
	string( REPLACE ".cpp" "" testname ${testsourcefile} )
	add_executable( ${testname} ${testsourcefile})	
	target_link_libraries( ${testname} MiniEngine )
endforeach( testsourcefile ${APP_SOURCES} )

# set_target_properties( MiniEngine PROPERTIES VS_PLATFORM_TOOLSET ClangCL )

target_link_libraries(MiniEngine ${CMAKE_SOURCE_DIR}/libs/glfw/lib-vc2022/glfw3.lib)
target_link_libraries(MiniEngine ${CMAKE_SOURCE_DIR}/libs/glfw/lib-vc2022/glfw3dll.lib)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_HEADERS})

# Add secondary tools

add_subdirectory(tools/Loader)
include_directories(tools/Loader/src)
target_link_libraries(MiniEngine Loader)

#install(TARGETS ${model-loader} RUNTIME DESTINATION bin)

# Get additional libraries

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm
	GIT_TAG 0.9.9.8
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
  add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_include_directories(MiniEngine PUBLIC ${glm_SOURCE_DIR}/glm)
target_link_libraries(MiniEngine glm)