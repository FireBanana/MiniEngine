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

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file( GLOB LIB_SOURCES 
	src/*.cpp
	src/components/*.cpp
	src/systems/*.cpp
	src/backend/*.cpp
	src/core/*.cpp
	src/core/utils/*.cpp
	src/core/types/*.cpp
	src/core/ui/*.cpp)

file( GLOB LIB_HEADERS 
	src/*.h
	src/components/*.h
	src/systems/*.h
	src/backend/*.h 
	src/core/*.h 
	src/core/utils/*.h
	src/core/types/*.h
	src/core/ui/*.h)

add_library( MiniEngine ${LIB_SOURCES} ${LIB_HEADERS} )
target_sources( MiniEngine PRIVATE ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c)

target_include_directories(MiniEngine PUBLIC libs)
target_include_directories(MiniEngine PUBLIC libs/glad/include)
target_include_directories(MiniEngine PUBLIC libs/glfw/include)

# Add secondary tools

add_subdirectory(tools/Loader)
add_subdirectory(examples/default)

target_include_directories(MiniEngine PUBLIC tools/Loader/src)
target_link_libraries(MiniEngine Loader)

#

target_include_directories(MiniEngine PUBLIC src/)
target_include_directories(MiniEngine PUBLIC src/components)
target_include_directories(MiniEngine PUBLIC src/systems)
target_include_directories(MiniEngine PUBLIC src/backend)
target_include_directories(MiniEngine PUBLIC src/core)
target_include_directories(MiniEngine PUBLIC src/core/utils)
target_include_directories(MiniEngine PUBLIC src/core/types)

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

#install(TARGETS ${model-loader} RUNTIME DESTINATION bin)

# Get additional libraries

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm
	GIT_TAG 0.9.9.8
)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui
	GIT_TAG v1.89.6
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
  add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
  FetchContent_Populate(imgui)
endif()

target_include_directories(MiniEngine PUBLIC ${glm_SOURCE_DIR}/glm)
target_link_libraries(MiniEngine glm)

target_include_directories(MiniEngine PUBLIC ${imgui_SOURCE_DIR}/)
target_include_directories(MiniEngine PUBLIC ${imgui_SOURCE_DIR}/backends)

add_library(IMGUI EXCLUDE_FROM_ALL 
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp 
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp 
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp)

target_include_directories(IMGUI PUBLIC libs/glfw/include)
target_include_directories(IMGUI PUBLIC ${imgui_SOURCE_DIR}/backends)
target_include_directories(IMGUI PUBLIC ${imgui_SOURCE_DIR}/)
target_link_libraries(MiniEngine IMGUI)