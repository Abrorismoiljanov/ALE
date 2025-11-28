cmake_minimum_required(VERSION 3.16)

# Project name and version
project(ALE VERSION 1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl2.cpp
)

# Add include directory
include_directories(include)
include_directories(imgui imgui/backends)

# Add source files
file(GLOB_RECURSE SOURCES ./src/*.cpp)


set(TINYFD_SOURCES src/tinyfiledialogs.c)
set_source_files_properties(${TINYFD_SOURCES} PROPERTIES LANGUAGE C)

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${TINYFD_SOURCES})


target_link_libraries(
    ${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    GLEW
    GL
)
