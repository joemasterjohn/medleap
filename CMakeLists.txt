cmake_minimum_required(VERSION 2.8)
project(medleap)


# output the executable in the build directory (not Debug/Release subdir)
# ******************************************
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)



# make sure resources are copied into the build directory
# ******************************************
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
    shaders/clut_strip.vert
    shaders/clut_strip.frag
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



# source group naming for IDEs
# ******************************************
source_group("shaders" FILES ${DATA_SHADERS})

file(GLOB SOURCE_GL src/gl/*.cpp src/gl/*.c src/gl/*.h)
source_group("gl" FILES ${SOURCE_GL})

file(GLOB SOURCE_MAIN src/main/*.cpp src/main/*.h)
source_group("main" FILES ${SOURCE_MAIN})

file(GLOB SOURCE_DATA src/data/*.cpp src/data/*.h)
source_group("data" FILES ${SOURCE_DATA})

file(GLOB SOURCE_MATH src/math/*.cpp src/math/*.h)
source_group("math" FILES ${SOURCE_MATH})

file(GLOB SOURCE_UTIL src/util/*.cpp src/util/*.h)
source_group("util" FILES ${SOURCE_UTIL})

file(GLOB SOURCE_LAYERS src/layers/*.cpp src/layers/*.h)
source_group("layers" FILES ${SOURCE_LAYERS})

file(GLOB SOURCE_LAYER_SLICE src/layers/slice/*.cpp src/layers/slice/*.h)
source_group("layers\\slice" FILES ${SOURCE_LAYER_SLICE})

file(GLOB SOURCE_LAYER_TFN1D src/layers/transfer_1D/*.cpp src/layers/transfer_1D/*.h)
source_group("layers\\transfer_1D" FILES ${SOURCE_LAYER_TFN1D})

file(GLOB SOURCE_LAYER_TFN2D src/layers/transfer_2D/*.cpp src/layers/transfer_2D/*.h)
source_group("layers\\transfer_2D" FILES ${SOURCE_LAYER_TFN2D})

file(GLOB SOURCE_LAYER_VOLUME src/layers/volume/*.cpp src/layers/volume/*.h)
source_group("layers\\volume" FILES ${SOURCE_LAYER_VOLUME})

file(GLOB SOURCE_LAYER_VOLINFO src/layers/volume_info/*.cpp src/layers/volume_info/*.h)
source_group("layers\\volume_info" FILES ${SOURCE_LAYER_VOLINFO})

file(GLOB SOURCE_LAYER_VOLORNT src/layers/volume_orientation/*.cpp src/layers/volume_orientation/*.h)
source_group("layers\\volume_orientation" FILES ${SOURCE_LAYER_VOLORNT})

# main executable
add_executable(
    ${PROJECT_NAME}
    ${SOURCE_MAIN}
    ${SOURCE_DATA}
    ${SOURCE_LAYERS}
    ${SOURCE_LAYER_SLICE}
    ${SOURCE_LAYER_TFN1D}
    ${SOURCE_LAYER_TFN2D}
    ${SOURCE_LAYER_VOLUME}
    ${SOURCE_LAYER_VOLINFO}
    ${SOURCE_LAYER_VOLORNT}
    ${SOURCE_GL}
    ${SOURCE_MATH}
    ${SOURCE_UTIL}
    ${DATA_SHADERS}
)
add_dependencies(${PROJECT_NAME} SHADER_COPY)

# set root of source directory for finding headers
include_directories(src)

# GDCM and GLFW should be installed on the system and are not included in source
find_package(GDCM REQUIRED)
include_directories(${GDCM_INCLUDE_DIRS})
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

if (APPLE)

    # for C++11
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

    # GLFW needs OpenGL, Cocoa, IOKit, and CoreVideo frameworks linked
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)

    # Link all the libraries
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
