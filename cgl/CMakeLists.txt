# CGL CMake File
# Justin Stoecker

cmake_minimum_required(VERSION 2.8)
project(CGL)

# group source by subdirectory names in any IDE projects
file(GLOB SRC_GL gl/*.cpp gl/*.h)
source_group("gl" FILES ${SRC_GL} ${INC_GL})
file(GLOB SRC_MATH math/*.h)
source_group("math" FILES ${SRC_MATH})
file(GLOB SRC_UTIL util/*.cpp util/*.h)
source_group("util" FILES ${SRC_UTIL})

# add include directories so the headers can be used without subdirectories
include_directories(.)
include_directories(gl)
include_directories(math)
include_directories(util)
include_directories(glew)

# target is a static library named cgl
add_library(cgl STATIC glew/glew.c ${SRC_GL} ${SRC_MATH} ${SRC_UTIL})
