cmake_minimum_required(VERSION 3.23.0)

project(editor)

add_executable(editor main.cpp)
add_compile_definitions(IMGUI_IMPL_VULKAN_USE_VOLK)

target_link_libraries(editor PUBLIC MiniEngine IMGUI)

target_include_directories(editor PUBLIC ${volk_SOURCE_DIR}/)
target_include_directories(editor PUBLIC ${imgui_SOURCE_DIR}/backends)
target_include_directories(editor PUBLIC ${imgui_SOURCE_DIR}/)
