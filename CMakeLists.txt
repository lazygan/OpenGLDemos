cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")

set(OpenGL_GL_PREFERENCE "LEGACY")
set(CMAKE_VERBOSE_MAKEFILE:BOOL "ON")
SET(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" CACHE STRING "" FORCE)

project(OpengGLDemos)
add_definitions(-w)
file(GLOB static_libraries  "${CMAKE_SOURCE_DIR}/depend/lib/*.lib")
include_directories(${CMAKE_SOURCE_DIR}/depend/include)
include_directories(${CMAKE_SOURCE_DIR}/depend/include/imgui-1.75)
include_directories(${CMAKE_SOURCE_DIR}/src/frame_work)
include_directories(${CMAKE_SOURCE_DIR}/src/)

add_executable(MAIN ./src/main.cpp)

target_link_libraries(MAIN PRIVATE ${static_libraries})
target_link_libraries(MAIN PRIVATE OpenGL32)
