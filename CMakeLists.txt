cmake_minimum_required (VERSION 3.8)
project ("PCKPP")

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/nativefiledialog-extended)

add_executable(PCKPP "src/main.cpp" "src/PCK/PCKAssetFile.h"  "src/PCK/PCKFile.h")

target_include_directories(PCKPP PRIVATE vendor/nativefiledialog-extended/src/include)
target_link_libraries(PCKPP PRIVATE nfd glfw)

target_include_directories(PCKPP PRIVATE vendor/glfw/include)

if (WIN32)
    target_compile_definitions(PCKPP PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
elseif (APPLE)
    target_compile_definitions(PCKPP PRIVATE GLFW_EXPOSE_NATIVE_COCOA)
elseif (UNIX)
    target_compile_definitions(PCKPP PRIVATE GLFW_EXPOSE_NATIVE_X11)
endif()

add_library(imgui
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
 "src/PCK/PCKAssetFile.h"  "src/PCK/PCKFile.h")

target_include_directories(imgui PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/vendor/imgui 
    ${CMAKE_CURRENT_LIST_DIR}/vendor/glfw/include
)
target_link_libraries(PCKPP PRIVATE imgui opengl32)

set_target_properties(PCKPP PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)