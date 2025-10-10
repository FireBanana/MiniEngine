cmake_minimum_required(VERSION 3.22.1)

project(MiniEngine 
    VERSION 1.0.0
    LANGUAGES C CXX
)

# ==================== Project Settings ====================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(GRAPHICS_DEBUG "Enable graphics debugging" ON)
option(RENDERDOC_ENABLE "Enable RenderDoc integration" OFF)

if(GRAPHICS_DEBUG)
    add_compile_definitions(GRAPHICS_DEBUG)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ==================== Find Required Packages ====================

find_package(Vulkan REQUIRED)

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

# ==================== Fetch Dependencies ====================

include(FetchContent)

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
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.8
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

# Configure GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# Configure spdlog
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)

# Configure glslang
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL ON CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)

# Configure assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

# Make dependencies available
FetchContent_MakeAvailable(glm spdlog glfw volk glslang vma assimp)

# ==================== MiniEngine Library ====================

file(GLOB_RECURSE PROJ_SRC 
    CONFIGURE_DEPENDS 
    "src/*.h" 
    "src/*.cpp"
)

file(GLOB_RECURSE SHADER_FILES 
    CONFIGURE_DEPENDS 
    "shaders/*.vert" 
    "shaders/*.frag"
)

add_library(MiniEngine ${PROJ_SRC})

# Add GLAD source
target_sources(MiniEngine PRIVATE 
    ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c
)

# Add Volk source
target_sources(MiniEngine PRIVATE 
    ${volk_SOURCE_DIR}/volk.c
)

# Platform-specific definitions
add_compile_definitions(GLFW_INCLUDE_NONE)
add_compile_definitions(USING_VULKAN)
add_compile_definitions(DIR="${CMAKE_SOURCE_DIR}")

if(WIN32)
    target_compile_definitions(MiniEngine PUBLIC VK_USE_PLATFORM_WIN32_KHR)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(MiniEngine PUBLIC VK_USE_PLATFORM_XLIB_KHR)
endif()

# Include directories
target_include_directories(MiniEngine PUBLIC
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/libs/glad/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/components
    ${CMAKE_SOURCE_DIR}/src/systems
    ${CMAKE_SOURCE_DIR}/src/backend
    ${CMAKE_SOURCE_DIR}/src/backend/vulkan
    ${CMAKE_SOURCE_DIR}/src/backend/framegraph
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/core/utils
    ${CMAKE_SOURCE_DIR}/src/core/types
    ${CMAKE_SOURCE_DIR}/shaders
)

# System include directories (suppress warnings)
target_include_directories(MiniEngine SYSTEM PUBLIC
    ${glm_SOURCE_DIR}
    ${spdlog_SOURCE_DIR}/include
    ${volk_SOURCE_DIR}
    ${glslang_SOURCE_DIR}
    ${vma_SOURCE_DIR}/include
    ${assimp_SOURCE_DIR}/include
)

# RenderDoc support
if(GRAPHICS_DEBUG AND RENDERDOC_ENABLE)
    if(DEFINED ENV{RENDERDOC})
        target_include_directories(MiniEngine SYSTEM PUBLIC $ENV{RENDERDOC})
    else()
        message(WARNING "RENDERDOC environment variable not set, skipping RenderDoc integration")
    endif()
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(MiniEngine PRIVATE 
        /W4 
        /external:anglebrackets
        /permissive-
    )
    # Disable specific warnings if needed
    target_compile_options(MiniEngine PRIVATE /wd4251 /wd4275)
else()
    target_compile_options(MiniEngine PRIVATE 
        -w
        # -Wall 
        # -Wextra 
        # -Wpedantic
    )
    
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(MiniEngine PRIVATE -g -ggdb -O0)
    endif()
    
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Link libraries
target_link_libraries(MiniEngine PUBLIC
    glm::glm
    spdlog::spdlog
    glfw
    Vulkan::Vulkan
    glslang
    SPIRV
    glslang-default-resource-limits
    assimp::assimp
)

# Platform-specific linking
if(UNIX AND NOT APPLE)
    target_link_libraries(MiniEngine PUBLIC
        ${X11_LIBRARIES}
        ${CMAKE_DL_LIBS}
        pthread
    )
endif()

# ==================== Application Executables ====================

file(GLOB APP_SOURCES 
    ${CMAKE_SOURCE_DIR}/app/*.cpp
    ${CMAKE_SOURCE_DIR}/app/*.cxx
)

foreach(app_source ${APP_SOURCES})
    get_filename_component(app_name ${app_source} NAME_WE)
    
    add_executable(${app_name} ${app_source})
    target_link_libraries(${app_name} PRIVATE MiniEngine)
    
    # Set output directory
    set_target_properties(${app_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# ==================== Tools ====================

add_subdirectory(tools/Loader)
add_subdirectory(tools/GlslCompiler)
add_subdirectory(examples/default)

target_include_directories(MiniEngine PUBLIC
    ${CMAKE_SOURCE_DIR}/tools/Loader/src
    ${CMAKE_SOURCE_DIR}/tools/GlslCompiler/src
)

target_link_libraries(MiniEngine PUBLIC
    Loader
    GlslCompiler
)

# ==================== Install Configuration (Optional) ====================

# Uncomment if you want install targets
# install(TARGETS MiniEngine
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
# )

# ==================== Summary ====================

message(STATUS "==============================================")
message(STATUS "MiniEngine Configuration Summary")
message(STATUS "==============================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Graphics Debug: ${GRAPHICS_DEBUG}")
message(STATUS "RenderDoc: ${RENDERDOC_ENABLE}")
message(STATUS "Vulkan SDK: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "==============================================")
