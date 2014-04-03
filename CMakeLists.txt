cmake_minimum_required(VERSION 2.8)
project(medleap)


# output the executable in the build directory (not Debug/Release subdir)
# -----------------------------------------------------------------------------------------------------------------------
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
# -----------------------------------------------------------------------------------------------------------------------


# make sure resources are copied into the build directory
# -----------------------------------------------------------------------------------------------------------------------
set(DATA_SHADERS
    shaders/color.vert
    shaders/color.frag
    shaders/slice_clut.vert
    shaders/slice_clut.frag
    shaders/volume_clut.vert
    shaders/volume_clut.frag
    shaders/histogram.vert
    shaders/histogram.frag
    shaders/histo_line.vert
    shaders/histo_line.frag
    shaders/texture_1D.vert
    shaders/texture_1D.frag
    shaders/texture_2D.vert
    shaders/texture_2D.frag
)
set(DATA_FONTS
    fonts/menlo14.bmp
    fonts/menlo14.dat
    fonts/menlo18.bmp
    fonts/menlo18.dat
    fonts/menlo24.bmp
    fonts/menlo24.dat
)
foreach(item IN LISTS DATA_SHADERS DATA_FONTS)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}" 
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
        DEPENDS   "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
    ) 
endforeach()
add_custom_target(SHADER_COPY ALL DEPENDS ${DATA_SHADERS} ${DATA_FONTS})
# -----------------------------------------------------------------------------------------------------------------------


# source group naming for IDEs
# -----------------------------------------------------------------------------------------------------------------------
source_group("" FILES src/main.cpp)
source_group("shaders" FILES ${DATA_SHADERS})

file(GLOB SOURCE_VOLUME src/volume/*.cpp src/volume/*.h)
source_group("volume" FILES ${SOURCE_VOLUME})

file(GLOB SOURCE_RENDER src/render/*.cpp src/render/*.h)
source_group("render" FILES ${SOURCE_RENDER})

file(GLOB SOURCE_UI src/ui/*.cpp src/ui/*.h)
source_group("ui" FILES ${SOURCE_UI})

file(GLOB SOURCE_GL src/gl/*.cpp src/gl/*.c src/gl/*.h)
source_group("gl" FILES ${SOURCE_GL})

file(GLOB SOURCE_MATH src/math/*.cpp src/math/*.h)
source_group("math" FILES ${SOURCE_MATH})

file(GLOB SOURCE_UTIL src/util/*.cpp src/util/*.h)
source_group("util" FILES ${SOURCE_UTIL})
# -----------------------------------------------------------------------------------------------------------------------


# main executable
# -----------------------------------------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${SOURCE_VOLUME}
    ${SOURCE_RENDER}
    ${SOURCE_UI}
    ${SOURCE_GL}
    ${SOURCE_MATH}
    ${SOURCE_UTIL}
    ${DATA_SHADERS}
)
add_dependencies(${PROJECT_NAME} SHADER_COPY)
# -----------------------------------------------------------------------------------------------------------------------


# libraries
# -----------------------------------------------------------------------------------------------------------------------
include_directories(src)

find_package(GDCM REQUIRED)
include_directories(${GDCM_INCLUDE_DIRS})
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

if (APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(
        ${PROJECT_NAME}
        gdcmMSFF
        ${GLFW_LIBRARY}
        ${OPENGL_LIBRARY}
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
endif(APPLE)
# -----------------------------------------------------------------------------------------------------------------------
