cmake_minimum_required(VERSION 3.22.1)
include(FetchContent)

project(MiniEngine)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)
set(CMAKE_RUNTIME_LIBRARY_DIRECTORY bin/)

set(GRAPHICS_DEBUG ON)
set(RENDERDOC_ENABLE OFF)
set(CMAKE_BUILD_TYPE "DEBUG")

add_definitions(-DGLFW_INCLUDE_NONE)
add_compile_definitions(DIR="${CMAKE_SOURCE_DIR}")

add_compile_definitions(USING_VULKAN)
if (WIN32)
        add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
else()
        add_compile_definitions(VK_USE_PLATFORM_XLIB_KHR)
endif()

if(GRAPHICS_DEBUG)
        add_compile_definitions(GRAPHICS_DEBUG)
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
        set(CMAKE_CXX_FLAGS_DEBUG  "-g")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file( GLOB_RECURSE PROJ_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "shaders/*.vert" "shaders/*.frag")
add_library( MiniEngine ${PROJ_SRC})

target_sources( MiniEngine PRIVATE ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c)

target_include_directories(MiniEngine PUBLIC libs)
target_include_directories(MiniEngine PUBLIC libs/glad/include)
target_include_directories(MiniEngine PUBLIC libs/glfw/include)
target_include_directories(MiniEngine PUBLIC src/)
target_include_directories(MiniEngine PUBLIC src/components)
target_include_directories(MiniEngine PUBLIC src/systems)
target_include_directories(MiniEngine PUBLIC src/backend)
target_include_directories(MiniEngine PUBLIC src/backend/vulkan)
target_include_directories(MiniEngine PUBLIC src/core)
target_include_directories(MiniEngine PUBLIC src/core/utils)
target_include_directories(MiniEngine PUBLIC src/core/types)
target_include_directories(MiniEngine PUBLIC shaders/)

target_include_directories(MiniEngine PUBLIC SYSTEM "$ENV{VULKAN_SDK}\\include")
message("PATH IS $ENV{VULKAN_SDK}")
message("Home dir is ${CMAKE_SOURCE_DIR}")

if(GRAPHICS_DEBUG)
    target_include_directories(MiniEngine PUBLIC SYSTEM $ENV{RENDERDOC}/) # Make sure RenderDoc installation is added to env var(RENDERDOC)
endif()

if(MSVC)
  target_compile_options(MiniEngine PRIVATE /W4 /external:anglebrackets)
else()
  # target_compile_options(MiniEngine PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(MiniEngine PRIVATE -w -g -ggdb -O0)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

file( GLOB APP_SOURCES app/*.cxx )

foreach( testsourcefile ${APP_SOURCES} )
        string( REPLACE ".cpp" "" testname ${testsourcefile} )
        add_executable( ${testname} ${testsourcefile})
        target_link_libraries( ${testname} MiniEngine )
endforeach( testsourcefile ${APP_SOURCES} )

# Get additional libraries

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8
)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.12.0
)

FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk
        GIT_TAG sdk-1.3.261.1
)

FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang
        GIT_TAG main
)

FetchContent_Declare(
        vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG master
)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG v5.4.3
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
  add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)
endif()

FetchContent_GetProperties(volk)
if(NOT volk_POPULATED)
  FetchContent_Populate(volk)
endif()

FetchContent_GetProperties(glslang)
if(NOT glslang_POPULATED)
  FetchContent_Populate(glslang)
endif()

FetchContent_GetProperties(vma)
if(NOT vma_POPULATED)
  FetchContent_Populate(vma)
endif()

FetchContent_MakeAvailable(assimp)

target_include_directories(MiniEngine PUBLIC SYSTEM ${glm_SOURCE_DIR}/glm)
target_link_libraries(MiniEngine glm)

target_include_directories(MiniEngine PUBLIC SYSTEM ${volk_SOURCE_DIR}/)
target_sources( MiniEngine PRIVATE ${volk_SOURCE_DIR}/volk.c)

set(SPDLOG_BUILD_SHARED ON CACHE BOOL "Build spdlog shared lib")
add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
target_include_directories(MiniEngine PUBLIC SYSTEM ${spdlog_SOURCE_DIR}/include/)
#target_link_libraries(MiniEngine spdlog)

target_include_directories(MiniEngine PUBLIC SYSTEM ${glslang_SOURCE_DIR}/glslang)

set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL ON CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
add_subdirectory(${glslang_SOURCE_DIR} ${glslang_BINARY_DIR})
target_link_libraries(MiniEngine glslang OSDependent SPIRV MachineIndependent GenericCodeGen glslang-default-resource-limits)
if(WIN32)
        target_link_libraries(MiniEngine ${CMAKE_SOURCE_DIR}/libs/glfw/lib-vc2022/libglfw3.a)
        target_link_libraries(MiniEngine ${CMAKE_SOURCE_DIR}/libs/glfw/lib-vc2022/libglfw3dll.a)
else()
        find_package(glfw3 REQUIRED)
        target_link_libraries(MiniEngine glfw)
endif()

target_include_directories(MiniEngine PUBLIC SYSTEM ${vma_SOURCE_DIR}/include)
#add_subdirectory(${vma_SOURCE_DIR} ${vma_BINARY_DIR})

target_link_libraries(MiniEngine assimp)
target_include_directories(MiniEngine PUBLIC SYSTEM ${assimp_SOURCE_DIR}/include)

# Add secondary tools

add_subdirectory(tools/Loader)
add_subdirectory(tools/GlslCompiler)
add_subdirectory(examples/default)

target_include_directories(MiniEngine PUBLIC tools/Loader/src)
target_include_directories(MiniEngine PUBLIC tools/GlslCompiler/src)

target_link_libraries(MiniEngine Loader)
target_link_libraries(MiniEngine GlslCompiler)

# ======
