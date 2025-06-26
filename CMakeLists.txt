cmake_minimum_required (VERSION 3.30.5)
project(PCKPP
        VERSION 1.0.0.0
        DESCRIPTION "A C++ Rewrite of PCK Studio, a tool to edit Minecraft Legacy Console Edition file formats!"
        LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_language(CXX)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
set(SRC ${SOURCE_FILES})

add_executable(PCKPP ${SRC})

find_package(OpenGL REQUIRED)
target_link_libraries(PCKPP PRIVATE OpenGL::GL)
target_link_libraries(PCKPP PRIVATE OpenGL::GLU)

add_subdirectory(vendor/glad)
target_link_libraries(PCKPP PRIVATE glad)

add_subdirectory(vendor/sdl)
target_link_libraries(PCKPP PRIVATE SDL3::SDL3)

add_library(imgui
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/backends/imgui_impl_opengl2.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui
    ${CMAKE_CURRENT_LIST_DIR}/vendor/stb
)

target_link_libraries(imgui PRIVATE SDL3::SDL3)

target_link_libraries(PCKPP PRIVATE imgui)

target_link_libraries(imgui PRIVATE opengl32)

set(SDL3_DLL "${CMAKE_BINARY_DIR}/vendor/SDL/SDL3.dll")

add_custom_command(TARGET PCKPP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL3_DLL}" $<TARGET_FILE_DIR:PCKPP>
)